## Test 01: Scripting Support [1 pts]

Trailing spaces should be eliminated by your command line tokenization process

```

expected=$(sh < "${TEST_DIR}/scripts/inspector.sh" 2> /dev/null)
actual=$(timeout 2 ./$SHELL_NAME < "${TEST_DIR}/scripts/inspector.sh" 2> /dev/null) || test_end

compare <(echo "${expected}") <(echo "${actual}")
echo "${expected}"

echo "${actual}"
Expected Output                                            | Actual Output
-----------------------------------------------------------V-----------------------------------------------------------
System Information                                            System Information
------------------                                            ------------------
Hostname:DESKTOP-6C3LURH                                      Hostname:DESKTOP-6C3LURH
Kernel Version:4.4.0-18362-Microsoft                          Kernel Version:4.4.0-18362-Microsoft
Logged in Users:                                              Logged in Users:
-----------------------------------------------------------^-----------------------------------------------------------

test_end
```

## Test 02: Basic process execution both from PATH and with absolute path [3 pts]

```

# ---------- Test Script ----------
echo "${script}"

/bin/echo System Information
/bin/echo ------------------
/bin/echo -n Hostname: 
cat /proc/sys/kernel/hostname
echo -n Kernel Version: 
cat /proc/sys/kernel/osrelease
echo Logged in Users:
who
 
# -------------- End --------------

expected=$(sh <(echo "${script}") 2> /dev/null)
actual=$(timeout 5 ./$SHELL_NAME < <(echo "${script}") 2> /dev/null)
if [[ $? -eq 124 ]]; then
    test_end 1 # Exit status 124 means execution timed out
fi

compare <(echo "${expected}") <(echo "${actual}")
echo "${expected}"

echo "${actual}"
Expected Output                                            | Actual Output
-----------------------------------------------------------V-----------------------------------------------------------
System Information                                            System Information
------------------                                            ------------------
Hostname:DESKTOP-6C3LURH                                      Hostname:DESKTOP-6C3LURH
Kernel Version:4.4.0-18362-Microsoft                          Kernel Version:4.4.0-18362-Microsoft
Logged in Users:                                              Logged in Users:
-----------------------------------------------------------^-----------------------------------------------------------

test_end
```

## Test 03: Long command lines [1 pts]

```

timeout 15 ./$SHELL_NAME < "${TEST_DIR}/scripts/too-long.sh" &> /dev/null || test_end 1
expected=$(sh < "${TEST_DIR}/scripts/long.sh" 2> /dev/null)
actual=$(timeout 15 ./$SHELL_NAME < "${TEST_DIR}/scripts/long.sh" 2> /dev/null) || test_end 1

compare <(echo "${expected}") <(echo "${actual}")
echo "${expected}"

echo "${actual}"
Expected Output                                            | Actual Output
-----------------------------------------------------------V-----------------------------------------------------------
1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 2    1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 2
done                                                          done
-----------------------------------------------------------^-----------------------------------------------------------

test_end
```

## Test 04: Comment support [1 pts]

```

# ---------- Test Script ----------
echo "${script}"
#Here's a comment
# Here's another
        # And another one! ####
echo Line 1 # This better not display
echo Line 2 #There's nothing here! #  echo No way
##################
# -------------- End --------------

expected=$(sh <(echo "${script}") 2> /dev/null)
actual=$(timeout 5 ./$SHELL_NAME < <(echo "${script}") 2> /dev/null) \
    || test_end
compare <(echo "${expected}") <(echo "${actual}")
echo "${expected}"

echo "${actual}"
Expected Output                                            | Actual Output
-----------------------------------------------------------V-----------------------------------------------------------
Line 1                                                        Line 1
Line 2                                                        Line 2
-----------------------------------------------------------^-----------------------------------------------------------

test_end
```

## Test 05: 'cd' built-in command [1 pts]

```

# ---------- Test Script ----------
echo "${script}"
oewiofjweoiwje
owfjeiowej
owjefiowejfiwoeewoijwio
ojwfe
cd /this/is/invalid
pwd
cd /usr/bin
./pwd
cd ../..
ls
cd
pwd
 
# -------------- End --------------

expected=$(sh <(echo "${script}") 2> /dev/null)
actual=$(timeout 5 ./$SHELL_NAME < <(echo "${script}") 2> /dev/null) \
    || test_end 1
compare <(echo "${expected}") <(echo "${actual}")
echo "${expected}"

echo "${actual}"
Expected Output                                            | Actual Output
-----------------------------------------------------------V-----------------------------------------------------------
/mnt/c/Users/andre/Documents/Coding/C/CS 326/Projects/cust    /mnt/c/Users/andre/Documents/Coding/C/CS 326/Projects/cust
bin                                                           bin
boot                                                          boot
dev                                                           dev
etc                                                           etc
home                                                          home
init                                                          init
lib                                                           lib
lib64                                                         lib64
media                                                         media
mnt                                                           mnt
opt                                                           opt
proc                                                          proc
root                                                          root
run                                                           run
sbin                                                          sbin
snap                                                          snap
srv                                                           srv
sys                                                           sys
tmp                                                           tmp
usr                                                           usr
var                                                           var
/home/secretbetta                                             /home/secretbetta
-----------------------------------------------------------^-----------------------------------------------------------

test_end
```

## Test 06: 'exit' built-in command [1 pts]

```

# ---------- Test Script ----------
echo "${script}"
echo Easiest point...
echo ...ever!
exit
echo If this prints, then exit
echo definitely isn't working!
# -------------- End --------------

expected=$(sh <(echo "${script}") 2> /dev/null)
actual=$(timeout 5 ./$SHELL_NAME < <(echo "${script}") 2> /dev/null) \
    || test_end 1
compare <(echo "${expected}") <(echo "${actual}")
echo "${expected}"

echo "${actual}"
Expected Output                                            | Actual Output
-----------------------------------------------------------V-----------------------------------------------------------
Easiest point...                                              Easiest point...
...ever!                                                      ...ever!
-----------------------------------------------------------^-----------------------------------------------------------

test_end
```

## Test 07: setenv built-in command [1 pts]

```

# ---------- Test Script ----------
echo "${script}"
setenv shell_test_variable_1 This-is-a-really-long-environment-variable-value
setenv lions
setenv shell_test_variable_2 Tigers
setenv shell_test_variable_3 Toitles
setenv
/mnt/c/Users/andre/Documents/Coding/C/CS 326/Projects/custom-shell/tests/scripts/print-env.sh
echo Test Complete
# -------------- End --------------

actual=$(timeout 5 ./$SHELL_NAME < <(echo "${script}") 2> /dev/null) \
    || test_end
compare <(echo "${expected}") <(echo "${actual}")
echo "${expected}"

echo "${actual}"
Expected Output                                            | Actual Output
-----------------------------------------------------------V-----------------------------------------------------------
This-is-a-really-long-environment-variable-value           <
Tigers                                                     <
Toitles                                                    <
Test Complete                                                 Test Complete
-----------------------------------------------------------^-----------------------------------------------------------

test_end
```

## Test 08: ^C should be ignored by the shell [1 pts]

NOTE: this test runs in scripting mode; make sure the prompt is not printed by SIGINT handler

```

output=$(timeout 5 ./$SHELL_NAME < <(echo "${script}") 2> /dev/null)

compare <(echo "It worked") <(echo "${output}")
echo "It worked"

echo "${output}"
Expected Output                                            | Actual Output
-----------------------------------------------------------V-----------------------------------------------------------
It worked                                                     It worked
-----------------------------------------------------------^-----------------------------------------------------------

test_end
```

## Test 09: History List [1 pts]

```

expected=$(HISTFILE=/dev/null HISTSIZE=100 bash \
    < "${TEST_DIR}/scripts/history-script-1.sh" 2> /dev/null)
actual=$(timeout 5 ./$SHELL_NAME < "${TEST_DIR}/scripts/history-script-1.sh" \
    2> /dev/null) || test_end
```

## Test 10: History Execution !num [1 pts]

```

expected=$(HISTFILE=/dev/null HISTSIZE=100 bash \
    < "${TEST_DIR}/scripts/history-script-2.sh" 2> /dev/null)
actual=$(timeout 5 ./$SHELL_NAME < "${TEST_DIR}/scripts/history-script-2.sh" \
    2> /dev/null) || test_end

compare <(echo "${expected}") <(echo "${actual}")
echo "${expected}"

echo "${actual}"
Expected Output                                            | Actual Output
-----------------------------------------------------------V-----------------------------------------------------------
Command 000                                                   Command 000
Command 001                                                   Command 001
Command 002                                                   Command 002
Command 003                                                   Command 003
Command 004                                                   Command 004
Command 005                                                   Command 005
Command 006                                                   Command 006
Command 007                                                   Command 007
Command 008                                                   Command 008
Command 009                                                   Command 009
Command 010                                                   Command 010
Command 011                                                   Command 011
Command 012                                                   Command 012
Command 013                                                   Command 013
Command 014                                                   Command 014
Command 015                                                   Command 015
Command 016                                                   Command 016
Command 017                                                   Command 017
Command 018                                                   Command 018
Command 019                                                   Command 019
Command 020                                                   Command 020
Command 021                                                   Command 021
Command 022                                                   Command 022
Command 023                                                   Command 023
Command 024                                                   Command 024
Command 025                                                   Command 025
Command 026                                                   Command 026
Command 027                                                   Command 027
Command 028                                                   Command 028
Command 029                                                   Command 029
Command 030                                                   Command 030
Command 031                                                   Command 031
Command 032                                                   Command 032
Command 033                                                   Command 033
Command 034                                                   Command 034
Command 035                                                   Command 035
Command 036                                                   Command 036
Command 037                                                   Command 037
Command 038                                                   Command 038
Command 039                                                   Command 039
Command 040                                                   Command 040
Command 041                                                   Command 041
Command 042                                                   Command 042
Command 043                                                   Command 043
Command 044                                                   Command 044
Command 045                                                   Command 045
Command 046                                                   Command 046
Command 047                                                   Command 047
Command 048                                                   Command 048
Command 049                                                   Command 049
Command 050                                                   Command 050
Command 051                                                   Command 051
Command 052                                                   Command 052
Command 053                                                   Command 053
Command 054                                                   Command 054
Command 055                                                   Command 055
Command 056                                                   Command 056
Command 057                                                   Command 057
Command 058                                                   Command 058
Command 059                                                   Command 059
Command 060                                                   Command 060
Command 061                                                   Command 061
Command 062                                                   Command 062
Command 063                                                   Command 063
Command 064                                                   Command 064
Command 065                                                   Command 065
Command 066                                                   Command 066
Command 067                                                   Command 067
Command 068                                                   Command 068
Command 069                                                   Command 069
Command 070                                                   Command 070
Command 071                                                   Command 071
Command 072                                                   Command 072
Command 073                                                   Command 073
Command 074                                                   Command 074
Command 075                                                   Command 075
Command 076                                                   Command 076
Command 077                                                   Command 077
Command 078                                                   Command 078
Command 079                                                   Command 079
Command 080                                                   Command 080
Command 081                                                   Command 081
Command 082                                                   Command 082
Command 083                                                   Command 083
Command 084                                                   Command 084
Command 085                                                   Command 085
Command 086                                                   Command 086
Command 087                                                   Command 087
Command 088                                                   Command 088
Command 089                                                   Command 089
Command 090                                                   Command 090
Command 091                                                   Command 091
Command 092                                                   Command 092
Command 093                                                   Command 093
Command 094                                                   Command 094
Command 095                                                   Command 095
Command 096                                                   Command 096
Command 097                                                   Command 097
Command 098                                                   Command 098
Command 099                                                   Command 099
Command 100                                                   Command 100
Command 101                                                   Command 101
Command 102                                                   Command 102
Command 103                                                   Command 103
Command 104                                                   Command 104
Command 105                                                   Command 105
Command 106                                                   Command 106
Command 107                                                   Command 107
Command 108                                                   Command 108
Command 109                                                   Command 109
Command 110                                                   Command 110
Command 111                                                   Command 111
Command 112                                                   Command 112
Command 113                                                   Command 113
Command 114                                                   Command 114
Command 115                                                   Command 115
Command 116                                                   Command 116
Command 117                                                   Command 117
Command 118                                                   Command 118
Command 119                                                   Command 119
Command 120                                                   Command 120
Command 121                                                   Command 121
Command 122                                                   Command 122
Command 123                                                   Command 123
Command 124                                                   Command 124
Command 125                                                   Command 125
Command 126                                                   Command 126
Command 127                                                   Command 127
Command 128                                                   Command 128
Command 129                                                   Command 129
Command 130                                                   Command 130
Command 131                                                   Command 131
Command 132                                                   Command 132
Command 133                                                   Command 133
Command 134                                                   Command 134
Command 135                                                   Command 135
Command 136                                                   Command 136
Command 137                                                   Command 137
Command 138                                                   Command 138
Command 139                                                   Command 139
Command 140                                                   Command 140
Command 141                                                   Command 141
Command 142                                                   Command 142
Command 143                                                   Command 143
Command 144                                                   Command 144
Command 145                                                   Command 145
Command 146                                                   Command 146
Command 147                                                   Command 147
Command 148                                                   Command 148
Command 149                                                   Command 149
Command 150                                                   Command 150
Command 151                                                   Command 151
Command 152                                                   Command 152
Command 153                                                   Command 153
Command 154                                                   Command 154
Command 155                                                   Command 155
Command 156                                                   Command 156
Command 157                                                   Command 157
Command 158                                                   Command 158
Command 159                                                   Command 159
Command 160                                                   Command 160
Command 161                                                   Command 161
Command 162                                                   Command 162
Command 163                                                   Command 163
Command 164                                                   Command 164
Command 165                                                   Command 165
Command 166                                                   Command 166
Command 167                                                   Command 167
Command 168                                                   Command 168
Command 169                                                   Command 169
Command 170                                                   Command 170
Command 171                                                   Command 171
Command 172                                                   Command 172
Command 173                                                   Command 173
Command 174                                                   Command 174
Command 175                                                   Command 175
Command 176                                                   Command 176
Command 177                                                   Command 177
Command 178                                                   Command 178
Command 179                                                   Command 179
Command 180                                                   Command 180
Command 181                                                   Command 181
Command 182                                                   Command 182
Command 183                                                   Command 183
Command 184                                                   Command 184
Command 185                                                   Command 185
Command 186                                                   Command 186
Command 187                                                   Command 187
Command 188                                                   Command 188
Command 189                                                   Command 189
Command 190                                                   Command 190
Command 191                                                   Command 191
Command 192                                                   Command 192
Command 193                                                   Command 193
Command 194                                                   Command 194
Command 195                                                   Command 195
Command 196                                                   Command 196
Command 197                                                   Command 197
Command 198                                                   Command 198
Command 199                                                   Command 199
Command 200                                                   Command 200
Command 201                                                   Command 201
Command 202                                                   Command 202
Command 203                                                   Command 203
Command 204                                                   Command 204
Command 205                                                   Command 205
Command 206                                                   Command 206
Command 207                                                   Command 207
Command 208                                                   Command 208
Command 209                                                   Command 209
Command 210                                                   Command 210
Command 211                                                   Command 211
Command 212                                                   Command 212
Command 213                                                   Command 213
Command 214                                                   Command 214
Command 215                                                   Command 215
Command 216                                                   Command 216
Command 217                                                   Command 217
Command 218                                                   Command 218
Command 219                                                   Command 219
Command 220                                                   Command 220
Command 221                                                   Command 221
Command 222                                                   Command 222
Command 223                                                   Command 223
Command 224                                                   Command 224
Command 225                                                   Command 225
Command 226                                                   Command 226
Command 227                                                   Command 227
Command 228                                                   Command 228
Command 229                                                   Command 229
Command 230                                                   Command 230
Command 231                                                   Command 231
Command 232                                                   Command 232
Command 233                                                   Command 233
Command 234                                                   Command 234
Command 235                                                   Command 235
Command 236                                                   Command 236
Command 237                                                   Command 237
Command 238                                                   Command 238
Command 239                                                   Command 239
Command 240                                                   Command 240
Command 241                                                   Command 241
Command 242                                                   Command 242
Command 243                                                   Command 243
Command 244                                                   Command 244
Command 245                                                   Command 245
Command 246                                                   Command 246
Command 247                                                   Command 247
Command 248                                                   Command 248
Command 249                                                   Command 249
Command 250                                                   Command 250
Command 251                                                   Command 251
Command 252                                                   Command 252
Command 253                                                   Command 253
Command 254                                                   Command 254
Command 255                                                   Command 255
Command 256                                                   Command 256
Command 257                                                   Command 257
Command 258                                                   Command 258
Command 259                                                   Command 259
Command 260                                                   Command 260
Command 261                                                   Command 261
Command 262                                                   Command 262
Command 263                                                   Command 263
Command 264                                                   Command 264
Command 265                                                   Command 265
Command 266                                                   Command 266
Command 267                                                   Command 267
Command 268                                                   Command 268
Command 269                                                   Command 269
Command 270                                                   Command 270
Command 271                                                   Command 271
Command 272                                                   Command 272
Command 273                                                   Command 273
Command 274                                                   Command 274
Command 275                                                   Command 275
Command 276                                                   Command 276
Command 277                                                   Command 277
Command 278                                                   Command 278
Command 279                                                   Command 279
Command 280                                                   Command 280
Command 281                                                   Command 281
Command 282                                                   Command 282
Command 283                                                   Command 283
Command 284                                                   Command 284
Command 285                                                   Command 285
Command 286                                                   Command 286
Command 287                                                   Command 287
Command 288                                                   Command 288
Command 289                                                   Command 289
Command 290                                                   Command 290
Command 291                                                   Command 291
Command 292                                                   Command 292
Command 293                                                   Command 293
Command 294                                                   Command 294
Command 295                                                   Command 295
Command 296                                                   Command 296
Command 297                                                   Command 297
Command 298                                                   Command 298
Command 299                                                   Command 299
Command 300                                                   Command 300
Command 301                                                   Command 301
Command 302                                                   Command 302
Command 303                                                   Command 303
Command 304                                                   Command 304
Command 305                                                   Command 305
Command 306                                                   Command 306
Command 307                                                   Command 307
Command 308                                                   Command 308
Command 309                                                   Command 309
Command 310                                                   Command 310
Command 311                                                   Command 311
Command 312                                                   Command 312
Command 313                                                   Command 313
Command 314                                                   Command 314
Command 315                                                   Command 315
Command 316                                                   Command 316
Command 317                                                   Command 317
Command 318                                                   Command 318
Command 319                                                   Command 319
Command 320                                                   Command 320
Command 321                                                   Command 321
Command 322                                                   Command 322
Command 323                                                   Command 323
Command 324                                                   Command 324
Command 325                                                   Command 325
Command 326                                                   Command 326
Command 327                                                   Command 327
Command 328                                                   Command 328
Command 329                                                   Command 329
Command 330                                                   Command 330
Command 331                                                   Command 331
Command 332                                                   Command 332
Command 333                                                   Command 333
Command 334                                                   Command 334
Command 335                                                   Command 335
Command 336                                                   Command 336
Command 337                                                   Command 337
Command 338                                                   Command 338
Command 339                                                   Command 339
Command 340                                                   Command 340
Command 341                                                   Command 341
Command 342                                                   Command 342
Command 343                                                   Command 343
Command 344                                                   Command 344
Command 345                                                   Command 345
Command 346                                                   Command 346
Command 347                                                   Command 347
Command 348                                                   Command 348
Command 349                                                   Command 349
Command 350                                                   Command 350
Command 351                                                   Command 351
Command 352                                                   Command 352
Command 353                                                   Command 353
Command 354                                                   Command 354
Command 355                                                   Command 355
Command 356                                                   Command 356
Command 357                                                   Command 357
Command 358                                                   Command 358
Command 359                                                   Command 359
Command 360                                                   Command 360
Command 361                                                   Command 361
Command 362                                                   Command 362
Command 363                                                   Command 363
Command 364                                                   Command 364
Command 365                                                   Command 365
Command 366                                                   Command 366
Command 367                                                   Command 367
Command 368                                                   Command 368
Command 369                                                   Command 369
Command 370                                                   Command 370
Command 371                                                   Command 371
Command 372                                                   Command 372
Command 373                                                   Command 373
Command 374                                                   Command 374
Command 375                                                   Command 375
Command 376                                                   Command 376
Command 377                                                   Command 377
Command 378                                                   Command 378
Command 379                                                   Command 379
Command 380                                                   Command 380
Command 381                                                   Command 381
Command 382                                                   Command 382
Command 383                                                   Command 383
Command 384                                                   Command 384
Command 385                                                   Command 385
Command 386                                                   Command 386
Command 387                                                   Command 387
Command 388                                                   Command 388
Command 389                                                   Command 389
Command 390                                                   Command 390
Command 391                                                   Command 391
Command 392                                                   Command 392
Command 393                                                   Command 393
Command 394                                                   Command 394
Command 395                                                   Command 395
Command 396                                                   Command 396
Command 397                                                   Command 397
Command 398                                                   Command 398
Command 399                                                   Command 399
Command 400                                                   Command 400
Command 401                                                   Command 401
Command 402                                                   Command 402
Command 403                                                   Command 403
Command 404                                                   Command 404
Command 405                                                   Command 405
Command 406                                                   Command 406
Command 407                                                   Command 407
Command 408                                                   Command 408
Command 409                                                   Command 409
Command 410                                                   Command 410
Command 411                                                   Command 411
Command 412                                                   Command 412
Command 413                                                   Command 413
Command 414                                                   Command 414
Command 415                                                   Command 415
Command 416                                                   Command 416
Command 417                                                   Command 417
Command 418                                                   Command 418
Command 419                                                   Command 419
-----                                                         -----
Command 419                                                   Command 419
Command 322                                                   Command 322
Command 398                                                   Command 398
-----------------------------------------------------------^-----------------------------------------------------------

test_end
```

## Test 11: History Execution !prefix [1 pts]

```

expected=$(HISTFILE=/dev/null HISTSIZE=100 bash \
    < "${TEST_DIR}/scripts/history-script-3.sh" 2> /dev/null)
actual=$(timeout 5 ./$SHELL_NAME < "${TEST_DIR}/scripts/history-script-3.sh" \
    2> /dev/null) || test_end

compare <(echo "${expected}") <(echo "${actual}")
echo "${expected}"

echo "${actual}"
Expected Output                                            | Actual Output
-----------------------------------------------------------V-----------------------------------------------------------
Command 000                                                   Command 000
Command 001                                                   Command 001
Linux DESKTOP-6C3LURH 4.4.0-18362-Microsoft #476-Microsoft    Linux DESKTOP-6C3LURH 4.4.0-18362-Microsoft #476-Microsoft
Command 003                                                   Command 003
Command 004                                                   Command 004
Command 005                                                   Command 005
Command 006                                                   Command 006
Command 007                                                   Command 007
Command 008                                                   Command 008
Command 009                                                   Command 009
Command 010                                                   Command 010
Command 011                                                   Command 011
Command 012                                                   Command 012
Command 013                                                   Command 013
Command 014                                                   Command 014
Command 015                                                   Command 015
Command 016                                                   Command 016
Command 017                                                   Command 017
Command 018                                                   Command 018
Command 019                                                   Command 019
Command 020                                                   Command 020
Command 021                                                   Command 021
Command 022                                                   Command 022
Command 023                                                   Command 023
Command 024                                                   Command 024
Command 025                                                   Command 025
Command 026                                                   Command 026
Command 027                                                   Command 027
Command 028                                                   Command 028
Command 029                                                   Command 029
Command 030                                                   Command 030
Command 031                                                   Command 031
Command 032                                                   Command 032
Command 033                                                   Command 033
Command 034                                                   Command 034
Command 035                                                   Command 035
Command 036                                                   Command 036
Command 037                                                   Command 037
Command 038                                                   Command 038
Command 039                                                   Command 039
Command 040                                                   Command 040
Command 041                                                   Command 041
Command 042                                                   Command 042
Command 043                                                   Command 043
Command 044                                                   Command 044
Command 045                                                   Command 045
Command 046                                                   Command 046
Command 047                                                   Command 047
Command 048                                                   Command 048
Command 049                                                   Command 049
Command 050                                                   Command 050
Command 051                                                   Command 051
Command 052                                                   Command 052
Command 053                                                   Command 053
Command 054                                                   Command 054
Command 055                                                   Command 055
Command 056                                                   Command 056
Command 057                                                   Command 057
Command 058                                                   Command 058
Command 059                                                   Command 059
Command 060                                                   Command 060
Command 061                                                   Command 061
Command 062                                                   Command 062
Command 063                                                   Command 063
Command 064                                                   Command 064
Command 065                                                   Command 065
Command 066                                                   Command 066
Command 067                                                   Command 067
Command 068                                                   Command 068
Command 069                                                   Command 069
Command 070                                                   Command 070
Command 071                                                   Command 071
Command 072                                                   Command 072
Command 073                                                   Command 073
Command 074                                                   Command 074
Command 075                                                   Command 075
Command 076                                                   Command 076
Command 077                                                   Command 077
Command 078                                                   Command 078
Command 079                                                   Command 079
Command 080                                                   Command 080
Command 081                                                   Command 081
Command 082                                                   Command 082
Command 083                                                   Command 083
Command 084                                                   Command 084
Command 085                                                   Command 085
Command 086                                                   Command 086
Command 087                                                   Command 087
Command 088                                                   Command 088
Command 089                                                   Command 089
Command 090                                                   Command 090
Command 091                                                   Command 091
Command 092                                                   Command 092
Command 093                                                   Command 093
Command 094                                                   Command 094
Command 095                                                   Command 095
Command 096                                                   Command 096
Command 097                                                   Command 097
Command 098                                                   Command 098
Command 099                                                   Command 099
Command 100                                                   Command 100
Command 101                                                   Command 101
Command 102                                                   Command 102
Command 103                                                   Command 103
Command 104                                                   Command 104
Command 105                                                   Command 105
Command 106                                                   Command 106
Command 107                                                   Command 107
Command 108                                                   Command 108
Command 109                                                   Command 109
Command 110                                                   Command 110
Command 111                                                   Command 111
Command 112                                                   Command 112
Command 113                                                   Command 113
Command 114                                                   Command 114
Command 115                                                   Command 115
Command 116                                                   Command 116
Command 117                                                   Command 117
Command 118                                                   Command 118
Command 119                                                   Command 119
Command 120                                                   Command 120
Command 121                                                   Command 121
Command 122                                                   Command 122
Command 123                                                   Command 123
Command 124                                                   Command 124
Command 125                                                   Command 125
Command 126                                                   Command 126
Command 127                                                   Command 127
Command 128                                                   Command 128
Command 129                                                   Command 129
Command 130                                                   Command 130
Command 131                                                   Command 131
Command 132                                                   Command 132
Command 133                                                   Command 133
Command 134                                                   Command 134
Command 135                                                   Command 135
Command 136                                                   Command 136
Command 137                                                   Command 137
Command 138                                                   Command 138
Command 139                                                   Command 139
Command 140                                                   Command 140
Command 141                                                   Command 141
Command 142                                                   Command 142
Command 143                                                   Command 143
Command 144                                                   Command 144
Command 145                                                   Command 145
Command 146                                                   Command 146
Command 147                                                   Command 147
Command 148                                                   Command 148
Command 149                                                   Command 149
Command 150                                                   Command 150
Command 151                                                   Command 151
Command 152                                                   Command 152
Command 153                                                   Command 153
Command 154                                                   Command 154
Command 155                                                   Command 155
Command 156                                                   Command 156
Command 157                                                   Command 157
Command 158                                                   Command 158
Command 159                                                   Command 159
Command 160                                                   Command 160
Command 161                                                   Command 161
Command 162                                                   Command 162
Command 163                                                   Command 163
Command 164                                                   Command 164
Command 165                                                   Command 165
Command 166                                                   Command 166
Command 167                                                   Command 167
Command 168                                                   Command 168
Command 169                                                   Command 169
Command 170                                                   Command 170
Command 171                                                   Command 171
Command 172                                                   Command 172
Command 173                                                   Command 173
Command 174                                                   Command 174
Command 175                                                   Command 175
Command 176                                                   Command 176
Command 177                                                   Command 177
Command 178                                                   Command 178
Command 179                                                   Command 179
Command 180                                                   Command 180
Command 181                                                   Command 181
Command 182                                                   Command 182
Command 183                                                   Command 183
Command 184                                                   Command 184
Command 185                                                   Command 185
Command 186                                                   Command 186
Command 187                                                   Command 187
Command 188                                                   Command 188
Command 189                                                   Command 189
Command 190                                                   Command 190
Command 191                                                   Command 191
Command 192                                                   Command 192
Command 193                                                   Command 193
Command 194                                                   Command 194
Command 195                                                   Command 195
Command 196                                                   Command 196
Command 197                                                   Command 197
Command 198                                                   Command 198
Command 199                                                   Command 199
Command 200                                                   Command 200
Command 201                                                   Command 201
Command 202                                                   Command 202
Command 203                                                   Command 203
Command 204                                                   Command 204
Command 205                                                   Command 205
Command 206                                                   Command 206
Command 207                                                   Command 207
Command 208                                                   Command 208
Command 209                                                   Command 209
Command 210                                                   Command 210
Command 211                                                   Command 211
Command 212                                                   Command 212
Command 213                                                   Command 213
Command 214                                                   Command 214
Command 215                                                   Command 215
Command 216                                                   Command 216
Command 217                                                   Command 217
Command 218                                                   Command 218
Command 219                                                   Command 219
Command 220                                                   Command 220
Command 221                                                   Command 221
Command 222                                                   Command 222
Command 223                                                   Command 223
Command 224                                                   Command 224
Command 225                                                   Command 225
Command 226                                                   Command 226
Command 227                                                   Command 227
Command 228                                                   Command 228
Command 229                                                   Command 229
Command 230                                                   Command 230
Command 231                                                   Command 231
Command 232                                                   Command 232
Command 233                                                   Command 233
Command 234                                                   Command 234
Command 235                                                   Command 235
Command 236                                                   Command 236
Command 237                                                   Command 237
Command 238                                                   Command 238
Command 239                                                   Command 239
Command 240                                                   Command 240
Command 241                                                   Command 241
Command 242                                                   Command 242
Command 243                                                   Command 243
Command 244                                                   Command 244
Command 245                                                   Command 245
Command 246                                                   Command 246
Command 247                                                   Command 247
Command 248                                                   Command 248
Command 249                                                   Command 249
Command 250                                                   Command 250
Command 251                                                   Command 251
Command 252                                                   Command 252
Command 253                                                   Command 253
Command 254                                                   Command 254
Command 255                                                   Command 255
Command 256                                                   Command 256
Command 257                                                   Command 257
Command 258                                                   Command 258
Command 259                                                   Command 259
Command 260                                                   Command 260
Command 261                                                   Command 261
Command 262                                                   Command 262
Command 263                                                   Command 263
Command 264                                                   Command 264
Command 265                                                   Command 265
Command 266                                                   Command 266
Command 267                                                   Command 267
Command 268                                                   Command 268
Command 269                                                   Command 269
Command 270                                                   Command 270
Command 271                                                   Command 271
Command 272                                                   Command 272
Command 273                                                   Command 273
Command 274                                                   Command 274
Command 275                                                   Command 275
Command 276                                                   Command 276
Command 277                                                   Command 277
Command 278                                                   Command 278
Command 279                                                   Command 279
Command 280                                                   Command 280
Command 281                                                   Command 281
Command 282                                                   Command 282
Command 283                                                   Command 283
Command 284                                                   Command 284
Command 285                                                   Command 285
Command 286                                                   Command 286
Command 287                                                   Command 287
Command 288                                                   Command 288
Command 289                                                   Command 289
Command 290                                                   Command 290
Command 291                                                   Command 291
Command 292                                                   Command 292
Command 293                                                   Command 293
Command 294                                                   Command 294
Command 295                                                   Command 295
Command 296                                                   Command 296
Command 297                                                   Command 297
Command 298                                                   Command 298
Command 299                                                   Command 299
Command 300                                                   Command 300
Command 301                                                   Command 301
Command 302                                                   Command 302
Command 303                                                   Command 303
Command 304                                                   Command 304
Command 305                                                   Command 305
Command 306                                                   Command 306
Command 307                                                   Command 307
Command 308                                                   Command 308
Command 309                                                   Command 309
Command 310                                                   Command 310
Command 311                                                   Command 311
Command 312                                                   Command 312
Command 313                                                   Command 313
Command 314                                                   Command 314
Command 315                                                   Command 315
Command 316                                                   Command 316
Command 317                                                   Command 317
Command 318                                                   Command 318
Command 319                                                   Command 319
Command 320                                                   Command 320
Command 321                                                   Command 321
Command 322                                                   Command 322
Command 323                                                   Command 323
Command 324                                                   Command 324
Command 325                                                   Command 325
Command 326                                                   Command 326
Command 327                                                   Command 327
Command 328                                                   Command 328
Command 329                                                   Command 329
Command 330                                                   Command 330
Command 331                                                   Command 331
Command 332                                                   Command 332
Command 333                                                   Command 333
Command 334                                                   Command 334
Command 335                                                   Command 335
Command 336                                                   Command 336
Command 337                                                   Command 337
Command 338                                                   Command 338
Command 339                                                   Command 339
Command 340                                                   Command 340
Command 341                                                   Command 341
Command 342                                                   Command 342
Command 343                                                   Command 343
Command 344                                                   Command 344
Command 345                                                   Command 345
Command 346                                                   Command 346
Command 347                                                   Command 347
Command 348                                                   Command 348
Command 349                                                   Command 349
Command 350                                                   Command 350
Command 351                                                   Command 351
Command 352                                                   Command 352
Command 353                                                   Command 353
Command 354                                                   Command 354
Command 355                                                   Command 355
Command 356                                                   Command 356
Command 357                                                   Command 357
Command 358                                                   Command 358
Command 359                                                   Command 359
Command 360                                                   Command 360
Command 361                                                   Command 361
Command 362                                                   Command 362
Command 363                                                   Command 363
Command 364                                                   Command 364
Command 365                                                   Command 365
Command 366                                                   Command 366
Command 367                                                   Command 367
Command 368                                                   Command 368
Command 369                                                   Command 369
Command 370                                                   Command 370
Command 371                                                   Command 371
init                                                          init
snap                                                          snap
srv                                                           srv
opt                                                           opt
media                                                         media
usr                                                           usr
lib64                                                         lib64
root                                                          root
var                                                           var
boot                                                          boot
mnt                                                           mnt
home                                                          home
bin                                                           bin
sbin                                                          sbin
lib                                                           lib
sys                                                           sys
proc                                                          proc
run                                                           run
etc                                                           etc
dev                                                           dev
tmp                                                           tmp
Command 373                                                   Command 373
Command 374                                                   Command 374
Command 375                                                   Command 375
Command 376                                                   Command 376
Command 377                                                   Command 377
Command 378                                                   Command 378
Command 379                                                   Command 379
Command 380                                                   Command 380
Command 381                                                   Command 381
HelloCommand 383                                              HelloCommand 383
Command 384                                                   Command 384
Command 385                                                   Command 385
Command 386                                                   Command 386
Command 387                                                   Command 387
Command 388                                                   Command 388
Command 389                                                   Command 389
Command 390                                                   Command 390
Command 391                                                   Command 391
Command 392                                                   Command 392
DESKTOP-6C3LURH                                               DESKTOP-6C3LURH
Command 394                                                   Command 394
Command 395                                                   Command 395
Command 396                                                   Command 396
Command 397                                                   Command 397
Command 398                                                   Command 398
Command 399                                                   Command 399
Command 400                                                   Command 400
Command 401                                                   Command 401
Command 402                                                   Command 402
Command 403                                                   Command 403
Command 404                                                   Command 404
Command 405                                                   Command 405
Command 407                                                   Command 407
Command 408                                                   Command 408
Command 409                                                   Command 409
Command 410                                                   Command 410
Command 411                                                   Command 411
Command 412                                                   Command 412
Command 413                                                   Command 413
Command 414                                                   Command 414
Command 415                                                   Command 415
Command 416                                                   Command 416
Command 417                                                   Command 417
Command 418                                                   Command 418
Command 419                                                   Command 419
-----                                                         -----
init                                                       <
snap                                                       <
srv                                                        <
opt                                                        <
media                                                      <
usr                                                        <
lib64                                                      <
root                                                       <
var                                                        <
boot                                                       <
mnt                                                        <
home                                                       <
bin                                                        <
sbin                                                       <
lib                                                        <
sys                                                        <
proc                                                       <
run                                                        <
etc                                                        <
dev                                                        <
tmp                                                        <
HelloDESKTOP-6C3LURH                                       <
-----                                                      <
-----------------------------------------------------------^-----------------------------------------------------------

test_end
```

## Test 12: History Execution !! [1 pts]

```

# ---------- Test Script ----------
echo "${script}"
set -o history    # Your shell can ignore this
set -o histexpand # Your shell can ignore this
ls -1 -t -r /
!!
uname -a
!!
oeiwiowejweo
!!
echo Goodbye
!!
# -------------- End --------------

expected=$(HISTFILE=/dev/null HISTSIZE=100 bash \
    < <(echo "${script}") 2> /dev/null)
actual=$(timeout 5 ./$SHELL_NAME < <(echo "${script}") 2> /dev/null) || test_end

compare <(echo "${expected}") <(echo "${actual}")
echo "${expected}"
echo "${actual}"

Expected Output                                            | Actual Output
-----------------------------------------------------------V-----------------------------------------------------------
init                                                          init
snap                                                          snap
srv                                                           srv
opt                                                           opt
media                                                         media
usr                                                           usr
lib64                                                         lib64
root                                                          root
var                                                           var
boot                                                          boot
mnt                                                           mnt
home                                                          home
bin                                                           bin
sbin                                                          sbin
lib                                                           lib
sys                                                           sys
proc                                                          proc
run                                                           run
etc                                                           etc
dev                                                           dev
tmp                                                           tmp
init                                                          init
snap                                                          snap
srv                                                           srv
opt                                                           opt
media                                                         media
usr                                                           usr
lib64                                                         lib64
root                                                          root
var                                                           var
boot                                                          boot
mnt                                                           mnt
home                                                          home
bin                                                           bin
sbin                                                          sbin
lib                                                           lib
sys                                                           sys
proc                                                          proc
run                                                           run
etc                                                           etc
dev                                                           dev
tmp                                                           tmp
Linux DESKTOP-6C3LURH 4.4.0-18362-Microsoft #476-Microsoft    Linux DESKTOP-6C3LURH 4.4.0-18362-Microsoft #476-Microsoft
Linux DESKTOP-6C3LURH 4.4.0-18362-Microsoft #476-Microsoft    Linux DESKTOP-6C3LURH 4.4.0-18362-Microsoft #476-Microsoft
Goodbye                                                       Goodbye
Goodbye                                                       Goodbye
-----------------------------------------------------------^-----------------------------------------------------------

test_end
```

## Test 13: Basic pipe support (single pipe) [1 pts]

```

# ---------- Test Script ----------
echo "${script}"
ls -1 / | sort
# -------------- End --------------

expected=$(sh <(echo "${script}") 2> /dev/null)
actual=$(./$SHELL_NAME < <(echo "${script}") 2> /dev/null) || test_end
compare <(echo "${expected}") <(echo "${actual}")
echo "${expected}"

echo "${actual}"
Expected Output                                            | Actual Output
-----------------------------------------------------------V-----------------------------------------------------------
bin                                                           bin
boot                                                          boot
dev                                                           dev
etc                                                           etc
home                                                          home
init                                                          init
lib                                                           lib
lib64                                                         lib64
media                                                         media
mnt                                                           mnt
opt                                                           opt
proc                                                          proc
root                                                          root
run                                                           run
sbin                                                          sbin
snap                                                          snap
srv                                                           srv
sys                                                           sys
tmp                                                           tmp
usr                                                           usr
var                                                           var
-----------------------------------------------------------^-----------------------------------------------------------

test_end
```

## Test 14: Pipe with a large amount of lines [1 pts]

```

# ---------- Test Script ----------
echo "${script}"
seq 10000000 | wc -l
# -------------- End --------------

expected=$(sh <(echo "${script}") 2> /dev/null)
actual=$(timeout 15 ./$SHELL_NAME < <(echo "${script}") 2> /dev/null) \
    || test_end
compare <(echo "${expected}") <(echo "${actual}")
echo "${expected}"

echo "${actual}"
Expected Output                                            | Actual Output
-----------------------------------------------------------V-----------------------------------------------------------
10000000                                                      10000000
-----------------------------------------------------------^-----------------------------------------------------------

test_end
```

## Test 15: Long pipeline (several pipes in a single command) [2 pts]

```

# ---------- Test Script ----------
echo "${script}"
cat /etc/passwd | sort | sed s/:.*//
echo -----
cat /etc/passwd | sort | sed s/:.*// | grep secretbetta
echo -----
cat /etc/passwd | sort | grep s | sed s/:.*// | tail -n 3 | head -n 2
echo -----
echo hi | cat | cat | cat | cat | sed s/h/y/g | sed s/i/o/g | shuf
# -------------- End --------------

expected=$(sh <(echo "${script}") 2> /dev/null)
actual=$(timeout 15 ./$SHELL_NAME < <(echo "${script}") 2> /dev/null) \
    || test_end
compare <(echo "${expected}") <(echo "${actual}")
echo "${expected}"

echo "${actual}"
Expected Output                                            | Actual Output
-----------------------------------------------------------V-----------------------------------------------------------
_apt                                                          _apt
backup                                                        backup
bin                                                           bin
daemon                                                        daemon
dnsmasq                                                       dnsmasq
games                                                         games
gnats                                                         gnats
irc                                                           irc
landscape                                                     landscape
list                                                          list
lp                                                            lp
lxd                                                           lxd
mail                                                          mail
man                                                           man
messagebus                                                    messagebus
news                                                          news
nobody                                                        nobody
pollinate                                                     pollinate
proxy                                                         proxy
root                                                          root
secretbetta                                                   secretbetta
sshd                                                          sshd
sync                                                          sync
sys                                                           sys
syslog                                                        syslog
systemd-network                                               systemd-network
systemd-resolve                                               systemd-resolve
uucp                                                          uucp
uuidd                                                         uuidd
www-data                                                      www-data
-----                                                         -----
secretbetta                                                   secretbetta
-----                                                         -----
uucp                                                          uucp
uuidd                                                         uuidd
-----                                                         -----
yo                                                            yo
-----------------------------------------------------------^-----------------------------------------------------------

test_end
```

## Test 16: Output redirection [1 pts]

This test writes several strings to files and ensures their results are correct. It also checks to make sure the permissions are set up correctly with open() -- use 0666.

```

# ---------- Test Script ----------
echo "${script}"
rm -f /mnt/c/Users/andre/Documents/Coding/C/CS 326/Projects/custom-shell/tests/testfile1 /mnt/c/Users/andre/Documents/Coding/C/CS 326/Projects/custom-shell/tests/testfile2
echo Hello world this is a nice long message to save to a file > /mnt/c/Users/andre/Documents/Coding/C/CS 326/Projects/custom-shell/tests/testfile1
cat /etc/services > /mnt/c/Users/andre/Documents/Coding/C/CS 326/Projects/custom-shell/tests/testfile2
echo This will overwrite the first message > /mnt/c/Users/andre/Documents/Coding/C/CS 326/Projects/custom-shell/tests/testfile1
cat /mnt/c/Users/andre/Documents/Coding/C/CS 326/Projects/custom-shell/tests/testfile1 /mnt/c/Users/andre/Documents/Coding/C/CS 326/Projects/custom-shell/tests/testfile2 > /mnt/c/Users/andre/Documents/Coding/C/CS 326/Projects/custom-shell/tests/final_output.txt
cat /mnt/c/Users/andre/Documents/Coding/C/CS 326/Projects/custom-shell/tests/final_output.txt
/mnt/c/Users/andre/Documents/Coding/C/CS 326/Projects/custom-shell/tests/scripts/get-stats.sh /mnt/c/Users/andre/Documents/Coding/C/CS 326/Projects/custom-shell/tests/final_output.txt
rm -f /mnt/c/Users/andre/Documents/Coding/C/CS 326/Projects/custom-shell/tests/final_output.txt
rm -f /mnt/c/Users/andre/Documents/Coding/C/CS 326/Projects/custom-shell/tests/testfile1 /mnt/c/Users/andre/Documents/Coding/C/CS 326/Projects/custom-shell/tests/testfile2
# -------------- End --------------

expected=$(sh <(echo "${script}") 2> /dev/null)
actual=$(./$SHELL_NAME < <(echo "${script}") 2> /dev/null) || test_end
compare <(echo "${expected}") <(echo "${actual}")
echo "${expected}"

echo "${actual}"
Expected Output                                            | Actual Output
-----------------------------------------------------------V-----------------------------------------------------------

-----------------------------------------------------------^-----------------------------------------------------------

test_end
```

## Test 17: Piping and redirecting a whole, whole lot of times... [1 pts]

```

# (script hidden for brevity)

expected=$(sh <(echo "${script}") 2> /dev/null)
actual=$(./$SHELL_NAME < <(echo "${script}") 2> /dev/null) || test_end
compare <(echo "${expected}") <(echo "${actual}")
echo "${expected}"

echo "${actual}"
Expected Output                                            | Actual Output
-----------------------------------------------------------V-----------------------------------------------------------

-----------------------------------------------------------^-----------------------------------------------------------

test_end
```

## Test 18: Background job support [1 pts]

```

# ---------- Test Script ----------
echo "${script}"
/mnt/c/Users/andre/Documents/Coding/C/CS 326/Projects/custom-shell/tests/scripts/sleeper.sh 500 &
/mnt/c/Users/andre/Documents/Coding/C/CS 326/Projects/custom-shell/tests/scripts/sleeper.sh 100 &
/mnt/c/Users/andre/Documents/Coding/C/CS 326/Projects/custom-shell/tests/scripts/sleeper.sh 85 &
/mnt/c/Users/andre/Documents/Coding/C/CS 326/Projects/custom-shell/tests/scripts/sleeper.sh 450 &
echo Done
/mnt/c/Users/andre/Documents/Coding/C/CS 326/Projects/custom-shell/tests/scripts/kill-parent.sh
# -------------- End --------------

actual=$(timeout 1 ./$SHELL_NAME < <(echo "${script}") 2> /dev/null) || test_end 1
```

## Test 19: Background job list [1 pts]

```

# ---------- Test Script ----------
echo "${script}"
sleep 1 &
sleep 2 &
sleep 500 &
sleep 100 &
sleep 450 &
sleep 3
jobs
/mnt/c/Users/andre/Documents/Coding/C/CS 326/Projects/custom-shell/tests/scripts/kill-parent.sh
# -------------- End --------------

actual=$(./$SHELL_NAME < <(echo "${script}") 2> /dev/null)

# We remove:
# - pids (unique on each system)
# - & (optional to display in the job list)
# - leading and trailing spaces
actual=$(awk '{$1=""; $0=$0; print $0}' <<< "${actual}" \
    | sed -e 's/&//g' -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//')

compare <(echo "${expected}") <(echo "${actual}")
echo "${expected}"

echo "${actual}"
Expected Output                                            | Actual Output
-----------------------------------------------------------V-----------------------------------------------------------
sleep 500                                                  |
sleep 100                                                  <
sleep 450                                                  <
-----------------------------------------------------------^-----------------------------------------------------------

test_end
```

## Test 20: Environment Variable Expansion [1 pts]

```

# ---------- Test Script ----------
echo "${script}"
echo SHELL is
echo $SHELL
echo PATH is
echo $PATH
echo TESTVAR is
echo $TESTVAR
# -------------- End --------------

expected=$(sh <(echo "${script}") 2> /dev/null)
actual=$(./$SHELL_NAME < <(echo "${script}") 2> /dev/null)

compare <(echo "${expected}") <(echo "${actual}")
echo "${expected}"
echo "${actual}"

Expected Output                                            | Actual Output
-----------------------------------------------------------V-----------------------------------------------------------
SHELL is                                                      SHELL is
/bin/bash                                                     /bin/bash
PATH is                                                       PATH is
/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/b <
TESTVAR is                                                 <
super awesome test variable                                <
-----------------------------------------------------------^-----------------------------------------------------------

test_end
```

## Test 21: Quote support [1 pts]

```

# ---------- Test Script ----------
echo "${script}"
/mnt/c/Users/andre/Documents/Coding/C/CS 326/Projects/custom-shell/tests/scripts/num-args.sh test1 test2 'this is a single quoted test'
/mnt/c/Users/andre/Documents/Coding/C/CS 326/Projects/custom-shell/tests/scripts/num-args.sh test1 test2 "this is a single quoted test"
/mnt/c/Users/andre/Documents/Coding/C/CS 326/Projects/custom-shell/tests/scripts/num-args.sh 'test1 test2'
/mnt/c/Users/andre/Documents/Coding/C/CS 326/Projects/custom-shell/tests/scripts/num-args.sh "test1 test2"
/mnt/c/Users/andre/Documents/Coding/C/CS 326/Projects/custom-shell/tests/scripts/num-args.sh 'test1 "test2" test3' "test4 'test5' test6"
/mnt/c/Users/andre/Documents/Coding/C/CS 326/Projects/custom-shell/tests/scripts/num-args.sh 'test'
/mnt/c/Users/andre/Documents/Coding/C/CS 326/Projects/custom-shell/tests/scripts/num-args.sh
echo "Take a look at these quotes"
echo 'Are they "amazing" or what?!'
# -------------- End --------------

expected=$(sh <(echo "${script}") 2> /dev/null)
actual=$(./$SHELL_NAME < <(echo "${script}") 2> /dev/null)

compare <(echo "${expected}") <(echo "${actual}")
echo "${expected}"

echo "${actual}"
Expected Output                                            | Actual Output
-----------------------------------------------------------V-----------------------------------------------------------
Take a look at these quotes                                   Take a look at these quotes
Are they "amazing" or what?!                                  Are they "amazing" or what?!
-----------------------------------------------------------^-----------------------------------------------------------

test_end
```

## Test 22: Environment Variable Expansion with Quotes [1 pts]

```

# ---------- Test Script ----------
echo "${script}"
echo "SHELL is $SHELL"
echo "PATH is $PATH"
echo "TESTVAR is $TESTVAR"
echo "And all of them together looks like: $SHELL $PATH $TESTVAR -- wow!"
echo "! $END !"
# -------------- End --------------

expected=$(sh <(echo "${script}") 2> /dev/null)
actual=$(./$SHELL_NAME < <(echo "${script}") 2> /dev/null)

compare <(echo "${expected}") <(echo "${actual}")
echo "${expected}"
echo "${actual}"

Expected Output                                            | Actual Output
-----------------------------------------------------------V-----------------------------------------------------------
SHELL is /bin/bash                                            SHELL is /bin/bash
PATH is /usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin: <
TESTVAR is super awesome test variable                     <
And all of them together looks like: /bin/bash /usr/local/ <
! The End !                                                <
-----------------------------------------------------------^-----------------------------------------------------------

test_end
```

## Test 23: Basic Memory Leak Check [1 pts]

```

# ---------- Test Script ----------
echo "${script}"
ls /
asdfghjklqprewopiqwualasdf # Bad Command!
# Comment only
pwd
# -------------- End --------------

valgrind --trace-children=no --leak-check=full \
    ./$SHELL_NAME < <(echo "${script}") 2>&1 \
    | grep 'are definitely lost'
echo "${script}"
==2488== 120 bytes in 1 blocks are definitely lost in loss record 3 of 6
==2484== 600 bytes in 5 blocks are definitely lost in loss record 1 of 1
[[ $? -eq 1 ]]

test_end
```

## Test 24: Thorough Memory Leak Check [1 pts]

```

valgrind --trace-children=no --leak-check=full \
    ./$SHELL_NAME < "${TEST_DIR}/scripts/leak.sh" 2>&1 \
    | grep 'are definitely lost'
==2504== 960 bytes in 8 blocks are definitely lost in loss record 4 of 6
==2635== 16,680 bytes in 139 blocks are definitely lost in loss record 4 of 6
[[ $? -eq 1 ]]

test_end
```

