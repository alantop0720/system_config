if has('mouse') 
set mouse-=a 
endif
set encoding=utf8
colorscheme desert
syntax on
set backspace=2
set bg=dark
set hlsearch
set laststatus=2
set cindent
set t_Co=256
set tabstop=4
set history=1000

call plug#begin()

" List your plugins here
Plug 'tpope/vim-sensible'
" Plug 'neoclide/coc.nvim', {'branch': 'release'}
Plug 'preservim/nerdtree'
" Plug 'itchyny/lightline.vim'
Plug 'vim-airline/vim-airline'
Plug 'vim-airline/vim-airline-themes'
call plug#end()


map <C-n> :NERDTreeToggle<CR>

nnoremap h ^
nnoremap l $
nnoremap j gg
nnoremap k G

" 将 F2 映射为切换到上一个 buffer
nnoremap <F2> :bprevious<CR>

" 将 F3 映射为切换到下一个 buffer
nnoremap <F3> :bnext<CR>


" 启用 tabline
let g:airline#extensions#tabline#enabled = 1
" 显示 buffer 编号
let g:airline#extensions#tabline#buffer_nr_show = 1
