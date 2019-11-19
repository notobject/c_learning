" vundle插件配置
set nocompatible
filetype off
set rtp+=~/.vim/bundle/Vundle.vim
call vundle#begin()

" add plugins here
Plugin 'scrooloose/nerdtree'
Plugin 'jiangmiao/auto-pairs'

call vundle#end()

" 基本配置
filetype plugin indent on
set number      " 显示行号
syntax enable   " 开启语法高亮
syntax on
set cursorline  " 高亮当前光标所在行
set ruler       " 在右下角显示当前光标所处的位置
set autoindent  " 设置自动缩进
set smartindent " 自动对齐
set tabstop=4   " 设置tab键为4列
set shiftwidth=4    " 换行时使用4列缩进，使用<或>缩进时使用4列
set softtabstop=4   " 退格时一次删除4个空格，不足4个空格，删除剩余空格
set expandtab   " TAB和缩进使用空格
set incsearch   " 查找：输入时定位匹配到的字符位置
set hlsearch    " 查找：高亮查找结果
set showcmd     " 在状态栏显示目前所执行的指令，未完成的指令片段亦会显示出来
set clipboard+=unnamed  " 与windows共享剪贴板
set path+=/usr/local/include/   " 添加文件查找目录
set path+=/usr/include
set tags=./tags;,tags           " 添加当前项目下的tags文件
set tags=~/tags  " 添加库文件的tags文件：ctags -R --c++-kinds=+p --fields=+iaS --extra=+q /usr/include/c++/

" 配色/主题
" set background=dark
" colorscheme solarized

" taglist
nnoremap <silent> <F2> :TlistToggle<CR>  " F2 打开
let Tlist_Show_One_File=1     " 只显示一个文件的taglist
let Tlist_Exit_OnlyWindow=1   " 当只剩taglist窗口时退出vim
let Tlist_Use_Right_Window=1  " 在右侧显示窗口
let Tlist_Sort_Type="name"    " 按tag的名字排序

" NERDTree
map <F3> :NERDTreeToggle<CR>  " F3打开
" 当执行vim时没有携带文件，自动打开NERDTree
autocmd StdinReadPre * let s:std_in=1
autocmd VimEnter * if argc() == 0 && !exists("s:std_in") | NERDTree | endif

" 状态条配置
set laststatus=2
set statusline=%y%f%m%=%{strftime(\"%Y/%m/%d\ %H:%M\")}[%{&ff},%{&fenc}]\ 0x%B\ %v/%{&textwidth}C,%l/%LL\ --%p%%--

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
    if &filetype == 'c' || &filetype == 'cpp' || &filetype == 'cxx'
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
    if &filetype == 'h'
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