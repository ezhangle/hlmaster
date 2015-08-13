1i\
<html>\
<head>\
<title></title>\
</head>\
<body>\
<pre>\


s/\([<>&]\)\1/\1/g
s/&/\&amp;/g
s/</\&lt;/g
s/>/\&gt;/g
s;_\(.\);<i>\1</i>;g
s;.\(.\);<b>\1</b>;g
s;</i><i>;;g
s;</b><b>;;g
s/	/    /g

# create email links
s;\([a-zA-z0-9\.\_]*\@[a-zA-z0-9\.\_]*\);<a href="mailto:\1">\1</a>;g

# create man links
/<b>[a-zA-Z0-9\-\_\.\+]*<\/b>([0-9])/{
s/<b>\([a-zA-Z0-9\-\_\.\+]*\)<\/b>/<b><a href=".\/\1\.html">\1<\/a><\/b>/g
}

# check for 'broken' links and mark them red (newline)
/<b>[a-zA-Z0-9\-\_\.\+]*\-<\/b>$/{
s/\(<b>[a-zA-Z0-9\-\_\.\+]*\-<\/b>\)/<font color=red>\1<\/font>/g
}


$a\
</pre>\
</body>\
</html>\
