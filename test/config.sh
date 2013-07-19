#!/bin/bash

die() {
    rm $(pwd)/tmp-build/test/c
    exit 1
}

echo 'simple = { b = true, str = "str", l = 100, d = 0.01 }' > $(pwd)/tmp-build/test/c || die
$(pwd)/tmp-build/test/config $(pwd)/tmp-build/test/c || die
$(pwd)/tmp-build/test/config || die

