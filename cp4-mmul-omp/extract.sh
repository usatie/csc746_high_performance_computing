#!/bin/bash

# ファイル名の引数チェック
if [ $# -ne 1 ]; then
	echo "Usage: $0 filename"
	exit 1
fi

# 入力ファイル名を変数に格納
filename=$1

# sedコマンドでパターンを抽出
sed -n -E '
  / 0 out of /p;

  /Region Info/{
    N;N;N;N;N;
    p
  }

  /Event/ {/HWThread 0/ { /HWThread 1/! {
    N;N;N;N;N;N;N;N;N;
    p
  } }}

  /Metric/ {/HWThread 0/ { /HWThread 1/! {
    N;N;N;N;N;N;N;N;
    p
  } }}

  /Event/ {/Sum/{
    N;N;N;N;N;N;N;N;N;
	/^\+|^\|/p
    }
  }

  /Metric/ {/Sum/{
    N;N;N;N;N;N;N;N;N;N;N;
	/^\+|^\|/p
    }
  }

' "$filename"
