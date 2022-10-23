# HW2_shogi
##### 電機4B 108501537 蔡雨蓁

**(A)new_game_file_name**

1.選擇毀棋,繼續,儲存棋譜 

Recover step input [0] | continue move input [1] | Save the chess [2] = 

毀棋 請輸入[0] --> 回到上一動

下一步移動 請輸入[1] --> a.移動符合規則 : 印出更新棋譜  b.移動不符合規則 : 重新輸入

儲存棋譜 請輸入[2] --> 棋譜寫入檔案"The_MOVE.txt", 遊戲結束

2.輸入移動位置

|請輸入移動棋子之[段]|&emsp; |請輸入移動棋子之[筋]|&emsp;&emsp;|請輸入欲移動位置之[段]|&emsp;&emsp; |請輸入欲移動位置之[筋]|&emsp;&emsp;   |
|------|--------|------|-----|----------------|-----|-----|------|

3.The program will show you **移動後的棋譜**和 **下棋時間**

4.重複新的回合


*************************************************************************

**(B)old_game_file_name**
|輸入[f]移動下一手|&emsp; |輸入[b]退回上一手|&emsp;&emsp;|輸入[e]結束程式|&emsp;&emsp;|
|------|--------|------|-----|----------------|-----|

## Compile & Run
```sh
# Compile
gcc -o main main.c -lev

# Run
1)開新局
./main -n -s new_game_file_name

2)載入棋譜
./main –l old_game_file_name
```
