#!/bin/bash

[ z $GUARD_CC ] && \
    GUARD_CC='/*\n    Copyright (C) \n*/\n\n'

find $@ -type f -name '*.h' -o -name '*.hxx' -o -name '*.hpp' | while read f
do
    ff=$(tr -- '-a-z./' '_A-Z__' <<< "$(pwd | sed 's!^.*/!!')__$f")
    m=$(md5sum $f)
    #echo $h -- $ff
    perl -i -pe 'BEGIN{undef $/;} s!^\s*(/\*.*\*/?)?\s*#\s*ifndef +\S+\s*#\s*define +\S*( +1)*\s*(\n *\S.*\S)\s*\n#endif *\S*\s*$|^(.*\S)\s*$!'"$GUARD_CC"'#ifndef '"$ff"'\n#define '"$ff"' 1\n$3$4\n\n#endif //'"$ff"'\n!smg' "$f"
    [ "$m" == "$(md5sum $f)" ] || echo $f
done

