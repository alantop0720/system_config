if has('mouse') 
set mouse-=a 
endif
set encoding=utf8
colorscheme desert
set nu
syntax on
set backspace=2
set bg=dark
set hlsearch
set laststatus=2
set cindent
set t_Co=256
set tabstop=4
set history=1000
nnoremap h ^
nnoremap l $
nnoremap j gg
nnoremap k G

set mouse=c

call plug#begin()

" List your plugins here
Plug 'tpope/vim-sensible'
" Plug 'neoclide/coc.nvim', {'branch': 'release'}
Plug 'preservim/nerdtree'
call plug#end()

map <C-n> :NERDTreeToggle<CR>


