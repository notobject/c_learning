" 显示行号
set number
" 开启语法高亮
syntax enable
syntax on
" 高亮当前光标所在行
set cursorline
" 在右下角显示当前光标所处的位置
set ruler
" 设置自动缩进
set autoindent
" 自动对齐
set smartindent
" 设置tab键为4列
set tabstop=4
" 换行时使用4列缩进，使用<或>缩进时使用4列
set shiftwidth=4
" 退格时一次删除4个空格，不足4个空格，删除剩余空格
set softtabstop=4
" TAB和缩进使用空格
set expandtab
" 查找：输入时定位匹配到的字符位置
set incsearch
" 查找：高亮查找结果
set hlsearch
" 在状态栏显示目前所执行的指令，未完成的指令片段亦会显示出来
set showcmd
" 开启文件类型自动检测
filetype on
" 允许加载文件类型插件
filetype plugin on
" 允许为不同类型的文件定义不同的缩进格式
filetype indent on
" 与windows共享剪贴板
set clipboard+=unnamed
" 添加文件查找目录
set path+=/usr/local/include/
set path+=/usr/include
" 添加tags文件路径
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

" 自动生成文件版权信息
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

" 状态条高度
set laststatus=2
" 在底部显示的状态条格式
set statusline=%y%f%m%=%{strftime(\"%Y/%m/%d\ %H:%M\")}[%{&ff},%{&fenc}]\ 0x%B\ %v/%{&textwidth}C,%l/%LL\ --%p%%--
