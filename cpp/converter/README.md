# pseudocode or uml or Something idk But. y'know.

## extracting the css

so obviously we have our basic version which hardcodes the regexps with `style type="text/css">` and `</style>`. which we should not be doing! but at the moment, it extracts the innerHTML of the `<style>` element correctly, which is good.

the contents of those lines should probably then be formatted neatly into css, just for kicksies. but probably also so that i can double check things.

in truth, the useful information will likely be pushed into a vector to be used when cleaning the html itself.

## cleaning the html

so currently, my thought is that. if a style ruling is deemed pointless, then its `<span class="s#">` element gets eliminated altogether. if it's considered useful, then it gets replaced with its more semantically relevant counterpart. 

if a span class is supposed to be made up of more than one semantically relevant rule. well then i will solve that some other day.

anyway, so after a purposeful span element is input, find the nearest `</span>` and replace its inner thing w/the correct element, and go back to just eliminating all `</span>`.

anyway, fortunately scrivener outputs each paragraph as one line of code.

## cleaning the output

there Will be a need to clean the output. there's a chance we'll end up w/misnesting, and ofc there's also the fact that scrivener (or html prettifiers) will sometimes add in weird spaces that makes punctuation Bad. and also will have to clean up escape characters n whatnot

there will probably need to be an Order to cleaning up escape characters, though all i can think of is "`&amp;` will probably have to be last."