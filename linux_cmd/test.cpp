#include <iostream>
#include <string>

std::string replaceBracketedContent(const std::string& input) {
    std::string output = input;
    size_t startPos = 0;
    while ((startPos = output.find('[', startPos))!= std::string::npos) {
        size_t endPos = output.find(']', startPos);
        if (endPos == std::string::npos) {
            break;
        }
        std::string key = output.substr(startPos + 1, endPos - startPos - 1);
        std::cout << "请输入 " << key << ": ";
        std::string value;
        std::cin >> value;
        output.replace(startPos, endPos - startPos + 1, value);
        startPos = endPos + value.size();
    }
    return output;
}

int main() {
    std::string input = "firewall-cmd --permanent --add-rich-rule=\"rule family=\"ipv4\" source address=\"[ip]\" port port=\"[port]\" protocol=\"tcp\" accept\"";
    std::string output = replaceBracketedContent(input);
    std::cout << output << std::endl;
    return 0;
}
