# scrivener to ao3 html converter

scrivener html export sucks ass. it has a bunch of stupid useless `<span>` elements, makes `<blockquotes>` unsemantically via css styling `<p>`, and can't even close off its `<ul>` and `<ol>` and `<li>` properly :/ absolute piece of shit garbage if you want to do literally anything other than basic paragraphs with bolds and italics, so i made this sweet little `.exe` file to help me with cleaning up the scrivener output when preparing to post to ao3.

## configuration options

the config file should be called `scriv2ao3.config` and placed in the same folder as the executable. lines starting with `#` will be considered comments and ignored. you cannot have inline comments bc idk how to make it read those and i don't feel like figuring it out.

the program will trim the spaces off your inputs, so you *could*, if you wanted, write them as `option = whatever`, but like. c'mon. just do it as `option=whatever` like a normal person.

|option|data type|elaboration|
|------|---------|-----------|
|`src`|**string**|**default:** *`html`*.<br>path to the folder with the html files to be converted. ideally relative to where the scriv2html executable is.|
|`deletesrc`|**bool**|**default:** *`false`*.<br>delete the original scrivener html export.|
|`copysrc`|**bool**`|**default:** *`false`*.<br>create a copy of the original scrivener html file elsewhere.|
|`copyfolder`|**string**|**default:** *`converted`*. <br>path to the folder where the original scrivener html files are to be transferred, if `copysrc` is set to `TRUE`.|
|`prettify`|**bool**|**default:** *`true`*.<br>prints the output with new lines and tabs for pretty readability when `TRUE`. otherwise, the output is one minified html mass. |
|`setfill`|**keyword**|**default:** *`\t`*. <br>sets the spacing character on prettified output. <br>**ACCEPTED KEYWORDS**: `tab` (`\t`); `space` (` `); all other input will result in setting the first character of the input string as the fill char. |
|`template`|**string**|**default:** *none*.<br>sets an html file as a template for the output. see #template section for more info when i make this a thing. |

## template

lol. haven't made this happen yet.