;errors check - first pass phase

;label errors
myLabel: .string "This should be fine"
1myLabel: .string "This shouldn't"
thisIsAveryVeryVeryVeryVeryLongLabel: .data 12, 4, 56, 78, 10
this label contains spaces in it: mov L1, L1
myLabel: .string "This label was already used on first line"

;comma errors
cmp K, , #-6
/add , L3  L3
inc , r1
inc, r1


;line length errors
K1: .data 1200, 1234, 54,90,-23         ,       42224,          3466,   +554,  -7,  12,75553, 763, 345
K2: .string "I am a very long string that will surely exceed the maximum length of a line"

;string errors
    .string za wardo
    .string za wardo"
    .string "za wardo


