if &cp | set nocp | endif
nnoremap  :b
map  h
let s:cpo_save=&cpo
set cpo&vim
noremap <NL> ddjP
noremap  ddkP
map  l
nnoremap  :bp
map <silent> ,V :source ~/.vimrc:filetype detect:exe ":echo 'vimrc reloaded'"
nmap ,c <Plug>Kwbd
nmap ,g :GitGutterToggle
nmap ,  :call whitespace#strip_trailing()
nmap ,] :TagbarToggle
nmap ,T :CtrlPClearCache:CtrlP
nmap ,t :CtrlP
nmap ,f :NERDTreeFind
nmap ,d :NERDTreeToggle
nmap ,b :CtrlPBuffer
nmap ,a :Ack 
map ,l :Align
vnoremap / /\v
nnoremap / /\v
xmap S <Plug>VSurround
vmap [% [%m'gv``
vmap ]% ]%m'gv``
vmap a% [%v]%
nmap cs <Plug>Csurround
nmap ds <Plug>Dsurround
nmap gx <Plug>NetrwBrowseX
xmap gS <Plug>VgSurround
nmap ySS <Plug>YSsurround
nmap ySs <Plug>YSsurround
nmap yss <Plug>Yssurround
nmap yS <Plug>YSurround
nmap ys <Plug>Ysurround
nnoremap <silent> <Plug>NetrwBrowseX :call netrw#NetrwBrowseX(expand("<cWORD>"),0)
nnoremap <silent> <Plug>SurroundRepeat .
nnoremap <Right> <Nop>
nnoremap <Left> <Nop>
nnoremap <Down> <Nop>
nnoremap <Up> <Nop>
imap S <Plug>ISurround
imap s <Plug>Isurround
imap  <Plug>Isurround
inoremap  u
inoremap jj 
cnoreabbr Q q
cnoreabbr W w
cnoreabbr WQ wq
cnoreabbr Wq wq
let &cpo=s:cpo_save
unlet s:cpo_save
set autoindent
set autoread
set backspace=indent,eol,start
set backupcopy=yes
set complete=.,w,b,u,t
set directory=~/tmp,/var/tmp,/tmp
set display=lastline
set expandtab
set fileencodings=ucs-bom,utf-8,default,latin1
set fileformats=unix,dos,mac
set helplang=en
set history=1000
set ignorecase
set incsearch
set laststatus=2
set listchars=tab:▸\ ,trail:·
set mouse=a
set nrformats=hex
set printoptions=paper:letter
set ruler
set runtimepath=~/.vim,~/.vim/bundle/vim-sensible,~/.vim/bundle/vim-surround,/var/lib/vim/addons,/usr/share/vim/vimfiles,/usr/share/vim/vim73,/usr/share/vim/vimfiles/after,/var/lib/vim/addons/after,~/.vim/after,~/.vim/bundle/powerline/powerline/bindings/vim
set scrolloff=3
set shiftround
set shiftwidth=2
set showcmd
set noshowmode
set sidescrolloff=5
set smartcase
set smarttab
set softtabstop=2
set suffixes=.bak,~,.swp,.o,.info,.aux,.log,.dvi,.bbl,.blg,.brf,.cb,.ind,.idx,.ilg,.inx,.out,.toc
set tabpagemax=50
set ttimeout
set ttimeoutlen=100
set viminfo=!,'100,<50,s10,h
set visualbell
set wildignore=log/**,node_modules/**,target/**,tmp/**,*.rbc
set wildmenu
set wildmode=longest,list,full
" vim: set ft=vim :
