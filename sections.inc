*   = $801
    .dsection basic
    .cerror * > $2000, "Basic section too long"

*   = $4000
    .dsection code
    .cerror * > $a000, "Code section too long"

    .align $100
    .dsection data
    .cerror * > $a000, "Data section too long"
