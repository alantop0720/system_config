package main

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"os"
	"os/exec"
	"strconv"
	"strings"
	"time"

	"github.com/shirou/gopsutil/cpu"
	"github.com/shirou/gopsutil/disk"
	"github.com/shirou/gopsutil/mem"
)

type SystemInfo struct {
	Hostname    string
	NetworkInfo []NetworkInterface
	CpuModel    string
	DiskSize    uint64
	DiskSerial  string
	DiskModel   string
	MemorySize  uint64
	MemoryInfo  []MemoryModule
	BoardModel  string
}

type NetworkInterface struct {
	Name        string
	Model       string
	Speed       string
	IPv4Address []string
	IPv4Mask    []string
	Gateway     string
	DNSServers  []string
	NTPServers  string
}

type MemoryModule struct {
	Manufacturer string
	PartNumber   string
	Capacity     uint64
	Speed        string
	BankLabel    string
	MemoryType   string
}

func getDiskSerial() string {
	cmd := exec.Command("wmic", "diskdrive", "get", "SerialNumber", "/value")
	output, err := cmd.Output()
	if err != nil {
		return "未知"
	}

	outputStr := string(output)
	if index := strings.Index(outputStr, "SerialNumber="); index != -1 {
		serialNumber := strings.TrimSpace(outputStr[index+13:])
		if serialNumber != "" {
			return serialNumber
		}
	}
	return "未知"
}

func getNetworkInfo() []NetworkInterface {
	var networks []NetworkInterface

	// 获取所有网络接口
	interfaces, err := net.Interfaces()
	if err != nil {
		return networks
	}

	for _, iface := range interfaces {
		// 跳过回环接口和未启用的接口
		if iface.Flags&net.FlagLoopback != 0 || iface.Flags&net.FlagUp == 0 {
			continue
		}

		netInfo := NetworkInterface{Name: iface.Name}

		// 获取网卡型号和速率
		cmd := exec.Command("wmic", "nic", "where", fmt.Sprintf("NetConnectionID='%s'", iface.Name),
			"get", "Name,Speed", "/value")
		output, err := cmd.Output()
		if err == nil {
			outputStr := string(output)

			// 获取型号
			if index := strings.Index(outputStr, "Name="); index != -1 {
				endIndex := strings.Index(outputStr[index:], "\r\n")
				if endIndex != -1 {
					netInfo.Model = strings.TrimSpace(outputStr[index+5 : index+endIndex])
				}
			}

			// 获取速率
			if index := strings.Index(outputStr, "Speed="); index != -1 {
				endIndex := strings.Index(outputStr[index:], "\r\n")
				if endIndex != -1 {
					speedStr := strings.TrimSpace(outputStr[index+6 : index+endIndex])
					if speed, err := strconv.ParseUint(speedStr, 10, 64); err == nil {
						netInfo.Speed = fmt.Sprintf("%.1f Gbps", float64(speed)/1000000000)
					}
				}
			}
		}

		// 获取IP地址和子网掩码
		addrs, err := iface.Addrs()
		if err != nil {
			continue
		}

		for _, addr := range addrs {
			if ipnet, ok := addr.(*net.IPNet); ok {
				if ip4 := ipnet.IP.To4(); ip4 != nil {
					netInfo.IPv4Address = append(netInfo.IPv4Address, ip4.String())
					netInfo.IPv4Mask = append(netInfo.IPv4Mask, net.IP(ipnet.Mask).String())
				}
			}
		}

		// 获取网关（使用 route print 命令）
		cmd = exec.Command("route", "print", "0.0.0.0")
		output, err = cmd.Output()
		if err == nil {
			lines := strings.Split(string(output), "\n")
			for _, line := range lines {
				if strings.Contains(line, "0.0.0.0") {
					fields := strings.Fields(line)
					if len(fields) > 2 {
						netInfo.Gateway = fields[2]
						break
					}
				}
			}
		}

		// 获取DNS服务器（从Windows注册表）
		cmd = exec.Command("powershell", "-Command",
			"Get-DnsClientServerAddress | Where-Object {$_.InterfaceAlias -eq '"+iface.Name+
				"'} | Select-Object -ExpandProperty ServerAddresses")
		output, err = cmd.Output()
		if err == nil {
			lines := strings.Split(string(output), "\n")
			for _, line := range lines {
				line = strings.TrimSpace(line)
				if line != "" && strings.Contains(line, ".") {
					netInfo.DNSServers = append(netInfo.DNSServers, line)
				}
			}
		}
		if len(netInfo.DNSServers) == 0 {
			netInfo.DNSServers = append(netInfo.DNSServers, "未配置")
		}

		// 获取NTP服务器（使用 PowerShell 命令并指定编码）
		cmd = exec.Command("reg", "query",
			"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\DateTime\\Servers")
		output, err = cmd.Output()
		if err == nil {
			lines := strings.Split(string(output), "\n")
			var ntpServers []string
			for _, line := range lines {
				line = strings.TrimSpace(line)
				if strings.HasPrefix(line, "0") || strings.HasPrefix(line, "1") || strings.HasPrefix(line, "2") {
					fields := strings.Fields(line)
					if len(fields) >= 3 {
						ntpServers = append(ntpServers, fields[2])
					}
				}
			}
			if len(ntpServers) > 0 {
				netInfo.NTPServers = strings.Join(ntpServers, ", ")
			} else {
				netInfo.NTPServers = "未配置"
			}
		} else {
			netInfo.NTPServers = "未知"
		}

		networks = append(networks, netInfo)
	}

	return networks
}

func getDiskModel() string {
	cmd := exec.Command("wmic", "diskdrive", "get", "Model", "/value")
	output, err := cmd.Output()
	if err != nil {
		return "未知"
	}

	outputStr := string(output)
	if index := strings.Index(outputStr, "Model="); index != -1 {
		model := strings.TrimSpace(outputStr[index+6:])
		if model != "" {
			return model
		}
	}
	return "未知"
}

func getMemoryInfo() []MemoryModule {
	var memoryModules []MemoryModule

	// 获取已安装的内存条信息
	cmd := exec.Command("wmic", "memorychip", "get", "Capacity,Manufacturer,Speed,SMBIOSMemoryType", "/value")
	output, err := cmd.Output()
	if err != nil {
		return memoryModules
	}

	outputStr := string(output)
	// 按 Capacity= 分割，因为每条内存信息都以 Capacity= 开头
	modules := strings.Split(outputStr, "Capacity=")
	// 跳过第一个空元素
	if len(modules) > 0 {
		modules = modules[1:]
	}

	for _, module := range modules {
		if strings.TrimSpace(module) == "" {
			continue
		}

		var mem MemoryModule
		// 解析容量
		if endIndex := strings.Index(module, "\r\n"); endIndex != -1 {
			if capacity, err := strconv.ParseUint(strings.TrimSpace(module[:endIndex]), 10, 64); err == nil {
				mem.Capacity = capacity
			}
		}

		// 获取制造商
		if index := strings.Index(module, "Manufacturer="); index != -1 {
			if endIndex := strings.Index(module[index:], "\n"); endIndex != -1 {
				mem.Manufacturer = strings.TrimSpace(module[index+13 : index+endIndex])
			}
		}

		// 获取速度
		if index := strings.Index(module, "Speed="); index != -1 {
			if endIndex := strings.Index(module[index:], "\n"); endIndex != -1 {
				speed := strings.TrimSpace(module[index+6 : index+endIndex])
				if speed != "" && speed != "0" {
					mem.Speed = speed + "MHz"
				}
			}
		}

		// 获取内存类型
		if index := strings.Index(module, "SMBIOSMemoryType="); index != -1 {
			if endIndex := strings.Index(module[index:], "\n"); endIndex != -1 {
				memType := strings.TrimSpace(module[index+17 : index+endIndex])
				ddrType := "DDR"
				switch memType {
				case "26": // DDR4
					ddrType = "DDR4"
				case "24": // DDR3
					ddrType = "DDR3"
				case "21": // DDR2
					ddrType = "DDR2"
				case "20": // DDR
					ddrType = "DDR"
				case "34": // DDR5
					ddrType = "DDR5"
				default:
					// 如果无法识别类型，尝试从速度判断
					if mem.Speed != "" {
						speed, _ := strconv.Atoi(strings.TrimSuffix(mem.Speed, "MHz"))
						switch {
						case speed >= 4800:
							ddrType = "DDR5"
						case speed >= 2133:
							ddrType = "DDR4"
						case speed >= 1066:
							ddrType = "DDR3"
						case speed >= 400:
							ddrType = "DDR2"
						}
					}
				}
				mem.MemoryType = ddrType
			}
		}

		if mem.Manufacturer != "" {
			memoryModules = append(memoryModules, mem)
		}
	}

	return memoryModules
}

func getMotherboardInfo() string {
	// 获取主板制造商
	cmd := exec.Command("wmic", "baseboard", "get", "Manufacturer,Product,Version", "/value")
	output, err := cmd.Output()
	if err != nil {
		return "未知"
	}

	outputStr := string(output)
	var boardInfo []string

	// 获取制造商
	if index := strings.Index(outputStr, "Manufacturer="); index != -1 {
		endIndex := strings.Index(outputStr[index:], "\n")
		if endIndex != -1 {
			manufacturer := strings.TrimSpace(outputStr[index+13 : index+endIndex])
			if manufacturer != "" {
				boardInfo = append(boardInfo, manufacturer)
			}
		}
	}

	// 获取产品型号
	if index := strings.Index(outputStr, "Product="); index != -1 {
		endIndex := strings.Index(outputStr[index:], "\n")
		if endIndex != -1 {
			product := strings.TrimSpace(outputStr[index+8 : index+endIndex])
			if product != "" {
				boardInfo = append(boardInfo, product)
			}
		}
	}

	// 获取版本号
	if index := strings.Index(outputStr, "Version="); index != -1 {
		endIndex := strings.Index(outputStr[index:], "\n")
		if endIndex != -1 {
			version := strings.TrimSpace(outputStr[index+8 : index+endIndex])
			if version != "" && version != "None" {
				boardInfo = append(boardInfo, version)
			}
		}
	}

	if len(boardInfo) > 0 {
		return strings.Join(boardInfo, " ")
	}
	return "未知"
}

func getSystemInfo() (*SystemInfo, error) {
	info := &SystemInfo{}

	// 获取主机名
	hostname, err := os.Hostname()
	if err != nil {
		return nil, err
	}
	info.Hostname = hostname

	// 获取网络信息
	info.NetworkInfo = getNetworkInfo()

	// 获取CPU信息
	cpuInfo, err := cpu.Info()
	if err != nil {
		return nil, err
	}
	if len(cpuInfo) > 0 {
		info.CpuModel = cpuInfo[0].ModelName
	}

	// 获取硬盘信息
	info.DiskSize = getTotalDiskSize()
	info.DiskSerial = getDiskSerial()
	info.DiskModel = getDiskModel()

	// 获取内存信息
	memInfo, err := mem.VirtualMemory()
	if err != nil {
		return nil, err
	}
	info.MemorySize = memInfo.Total
	info.MemoryInfo = getMemoryInfo()
	info.BoardModel = getMotherboardInfo()

	return info, nil
}

func getTotalDiskSize() uint64 {
	var totalSize uint64 = 0
	partitions, err := disk.Partitions(false)
	if err != nil {
		return 0
	}

	for _, partition := range partitions {
		if len(partition.Device) > 0 && strings.Contains(partition.Device, ":") {
			usage, err := disk.Usage(partition.Mountpoint)
			if err != nil {
				continue
			}
			totalSize += usage.Total
		}
	}
	return totalSize
}

func main() {
	// 获取系统信息
	sysInfo, err := getSystemInfo()
	if err != nil {
		log.Fatal(err)
	}

	// 格式化系统信息
	hdinfo := "硬件配置信息：\n"
	hdinfo += fmt.Sprintf("主板型号：%s\n", sysInfo.BoardModel)
	hdinfo += fmt.Sprintf("CPU型号：%s\n", sysInfo.CpuModel)

	// 添加网卡型号信息
	hdinfo += "网卡型号：\n"
	for _, net := range sysInfo.NetworkInfo {
		if net.Model != "" {
			hdinfo += fmt.Sprintf("    %s：%s", net.Name, net.Model)
			if net.Speed != "" {
				hdinfo += fmt.Sprintf(" (%s)", net.Speed)
			}
			hdinfo += "\n"
		}
	}
	hdinfo += "\n"

	// 添加内存配置览
	hdinfo += fmt.Sprintf("内存总容量：%.1f GB\n", float64(sysInfo.MemorySize)/(1024*1024*1024))

	// 计算已安装的内存条数量
	installedCount := 0
	var memDetails strings.Builder
	for _, mem := range sysInfo.MemoryInfo {
		if mem.Manufacturer != "" || mem.PartNumber != "" {
			installedCount++
			memDetails.WriteString(fmt.Sprintf("    %d号内存：%s %.1fGB %s %s\n",
				installedCount,
				mem.Manufacturer,
				float64(mem.Capacity)/(1024*1024*1024),
				mem.Speed,
				mem.MemoryType))
		}
	}
	hdinfo += fmt.Sprintf("已安装内存条数量：%d\n", installedCount)
	hdinfo += memDetails.String()

	hdinfo += fmt.Sprintf("硬盘型号：%s\n", sysInfo.DiskModel)
	hdinfo += fmt.Sprintf("硬盘容量：%.2f GB\n", float64(sysInfo.DiskSize)/(1024*1024*1024))
	hdinfo += fmt.Sprintf("硬盘序列号：%s\n", sysInfo.DiskSerial)

	// 添加网卡信息概览
	hdinfo += "网卡信息：\n"
	for _, net := range sysInfo.NetworkInfo {
		hdinfo += fmt.Sprintf("  %s：\n", net.Name)
		for i, ip := range net.IPv4Address {
			hdinfo += fmt.Sprintf("    IP地址：%s\n", ip)
			hdinfo += fmt.Sprintf("    子网掩码：%s\n", net.IPv4Mask[i])
		}
	}

	hdinfo += "\n系统信息：\n"
	hdinfo += fmt.Sprintf("主机名：%s\n", sysInfo.Hostname)

	// 添加网络详细信息
	hdinfo += "网络配置：\n"
	for _, net := range sysInfo.NetworkInfo {
		hdinfo += fmt.Sprintf("接口名称：%s\n", net.Name)
		hdinfo += fmt.Sprintf("  网关：%s\n", net.Gateway)
		hdinfo += "  DNS服务器：\n"
		for _, dns := range net.DNSServers {
			hdinfo += fmt.Sprintf("    %s\n", dns)
		}
		hdinfo += fmt.Sprintf("  NTP服务器：%s\n", net.NTPServers)
	}

	// 获取当前时间
	currentTime := time.Now().Format("2006-01-02 15:04:05")

	// 准备写入的内容
	content := fmt.Sprintf("采集时间：%s\n%s\n\n", currentTime, hdinfo)

	// 以追加模式打开文件
	file, err := os.OpenFile("info.txt", os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
	if err != nil {
		log.Fatal(err)
	}
	defer file.Close()

	// 写入内容
	if _, err := file.WriteString(content); err != nil {
		log.Fatal(err)
	}

	fmt.Println("系统信息已成功写入 info.txt")
	fmt.Println(hdinfo)

	// 等待用户按回车键退出
	fmt.Print("\n按回车键退出...")
	reader := bufio.NewReader(os.Stdin)
	reader.ReadString('\n')
}
