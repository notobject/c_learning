set number
syntax on
set cursorline
set ruler
set autoindent
set smartindent
set tabstop=4
set shiftwidth=4
set softtabstop=4
set expandtab
set incsearch
set hlsearch
set showcmd
filetype on
filetype plugin on
filetype indent on
set path+=/usr/local/include/
set path+=/usr/include
set tags=./tags;,tags

" 自动补全花括号
:inoremap { {<CR>}<ESC>O
:inoremap } <c-r>=ClosePair('}')<CR>

function! ClosePair(char)
    if getline('.')[col('.') - 1] == a:char
        return "\<Right>"
    else
        return a:char
    endif
endfunction

" 颜色主题
set background=dark
colorscheme solarized
" colorscheme one

" 文件版权信息
autocmd BufNewFile *.py,*.cpp,*.[ch],*.sh,*.java exec ":call SetTitle()"
func SetTitle()
    if &filetype == 'sh'
        call setline(1, "\#!/bin/bash")
        call append(line(".")  , "\# File Name : ".expand("%"))
        call append(line(".")+1, "\# Created By: Long Duping")
        call append(line(".")+2, "\# Created At: ".strftime("%c"))
        call append(line(".")+3, "\# @breif    : ")
    endif
    if &filetype == 'c'
        call setline(1         , "/**************************************************************")
        call append(line(".")  , " *")
        call append(line(".")+1, " * File Name : ".expand("%"))
        call append(line(".")+2, " * Created By: Long Duping")
        call append(line(".")+3, " * Created At: ".strftime("%Y-%m-%d %H:%M:%S"))
        call append(line(".")+4, " * @breif    : ")
        call append(line(".")+5, " *")
        call append(line(".")+6, " **************************************************************/")
        call append(line(".")+7, "")
    endif
    if &filetype == 'cpp'
        call setline(1         , "/**************************************************************")
        call append(line(".")  , " *")
        call append(line(".")+1, " * File Name : ".expand("%"))
        call append(line(".")+2, " * Created By: Long Duping")
        call append(line(".")+3, " * Created At: ".strftime("%Y-%m-%d %H:%M:%S"))
        call append(line(".")+4, " * @breif    : ")
        call append(line(".")+5, " *")
        call append(line(".")+6, "**************************************************************/")
        call append(line(".")+7, "")
        call append(line(".")+8, "#ifndef __".toupper(expand("%:t:r"))."_H")
        call append(line(".")+9, "#define __".toupper(expand("%:t:r"))."_H")
        call append(line(".")+10, "")
        call append(line(".")+11, "")
        call append(line(".")+12, "")
        call append(line(".")+13, "#ifdef __cplusplus")
        call append(line(".")+14, "extern \"C\" {")
        call append(line(".")+15, "#endif") 
        call append(line(".")+16, "")
        call append(line(".")+17, "")
        call append(line(".")+18, "")
        call append(line(".")+19, "#ifdef __cplusplus")
        call append(line(".")+20, "};")
        call append(line(".")+21, "#endif")
        call append(line(".")+22, "")
        call append(line(".")+23, "#endif"."/* endif __".toupper(expand("%:t:r"))."_H"." */")
    endif
    if &filetype == "py"
        call setline(1, "\#!/usr/bin/python")
        call append(line("."), "\# -*- encoding=utf-8 -*-")
    endif
    autocmd BufNewFile * normal G

endfunc

" 状态条
set laststatus=2
set statusline=%y%f%m%=%{strftime(\"%Y/%m/%d\ %H:%M\")}[%{&ff},%{&fenc}]\ 0x%B\ %v/%{&textwidth}C,%l/%LL\ --%p%%--
