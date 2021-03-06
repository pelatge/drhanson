#ifdef TeXinput
\input loommac
\def\textindent#1{\noindent#1\thinspace\ignorespaces}
\def\Unix{U{\sc NIX}}
\def\UPM{{\sl U{\eightsl NIX} Programmer's Manual}}

\centerline{\eleventt common\bf: Printing Common Words%
\footnote\dag{\ninepoint This work was supported in part by the National
Science Foundation under Grant MCS--8302398.}}
\bigskip\centerline{\rm David R.\ Hanson}
\centerline{\it Department of Computer Science, Princeton University,
Princeton, NJ 08544}
\smallskip\centerline{\rm July 31, 1987}
\bigskip\bigskip

\section Introduction.

In describing Don Knuth's |WEB| system in one
of his ``Programming Pearls'' [{\it Communications of the ACM\/ \bf 29}(5),
364--369, May 1986], Jon Bentley ``assigned'' the following programming
problem:
\medskip
{\narrower\narrower\noindent
Given a text file and an integer $k$, you are to print
the $k$ most common words in the file (and the number
of their occurrences) in decreasing frequency.
\medskip}
It is unclear from this problem statement what to do with ``ties'',
that is, does $k$ refer to words or word frequencies?
For example, in the problem statement,
``the'' occurs three times, ``$k$'', ``in'', ``and'', and ``file''
each occur twice, and the rest of the words each occur once.
If the program is invoked with the statement as input and $k=2$,
which word should be output as the second most common word?
Rephrasing the problem statement removes the ambiguity:
\medskip
{\narrower\narrower\noindent
Given a text file and an integer $k$, you are to print
the words (and their frequencies of occurrence) whose
frequencies of occurrence are among the $k$ largest
in order of decreasing frequency.
\medskip}
Using this problem statement, the output of the program
with the original problem statement as input and with $k=2$ is
\begindisplay\tt
3 the
2 file
2 and
2 in
2 k
\enddisplay

Bentley assigned this problem to Don Knuth in order to present
a ``real'' example of |WEB| usage. For more information about |WEB|, see
D.~E.\ Knuth, `Literate Programming', {\it The Computer Journal\/ \bf 27}(2),
97--111, May 1984.

The solution given here is written in the C programming
language and presented using the |loom| system to generate
the printed program and its explanation. |loom| is a
preprocessor whose input is a text file with
embedded references to fragments of the program.
|loom| retrieves these fragments, optionally pushes
them through arbitrary filters, and integrates the result into the output.

|loom|'s output is usually input to a document formatter,
such as |troff| or \TeX. |loom| was originally written
by Janet Incerpi and Robert Sedgewick at Brown University
and used in preparation of Sedgewick's book {\sl Algorithms\/}
(Addison-Wesley, 1983). Starting from their program,
I rewrote |loom| for use in writing my book and papers.

|loom| is not as ambitious or as comprehensive as |WEB|.
It does, however, have the virtue of independence
from both formatting and programming languages.
It does not, for example, provide the comprehensive indexing,
cross referencing, or pretty printing facilities of |WEB|.
With help from its associated filters, |loom| does provide indexing
of the identifiers used in the program fragments.
I have formatted this program description in a style
similar to |WEB| for comparison purposes,
but the formatting of |loom|'s output is not constrained to any one style.
Using |loom| also has an effect similar to |WEB|: developing and
writing about programs concurrently affects both activities dramatically.

\section Definitions.

The problem statement does not give a precise definition
of a ``word'' nor of the details of program invocation.
{\it Words\/} are given by the set
$\{w \mid w = aa^*\ {\rm and}\ \left\vert w\right\vert \leq 100\}$
where $a \in \{|a|\cdots|z|, |A|\cdots|Z|\}$;
i.e., a word is a sequence of one or more upper- or lower-case letters,
up to a maximum of 100 letters. Only the first 100 characters are considered
for words longer than 100 characters.

The program, called |common|, is invoked with a single optional
argument that gives the value of $k$ and reads its input
from the standard input file. If the argument is
omitted, the value of the environment variable |PAGESIZE|
is used. In the absence of |PAGESIZE|, $k$ is 22.

\section The Main Program.

As suggested in {\sl Software Tools\/} by Kernighan and Plauger
(Addison-Wesley, 1976),
the overall structure of the program can often be derived
from the overall structure of the input data.
The input to |common| is a sequence of zero or more words, which suggests
the following structure for the main program:
\beginprogram
/* initialize k */
/* initialize word table */
%include common.c mainprogram sed 's/^	//' | prog | xref +buf +k
\endprogram
where |buf| is a character array capable of holding 
|MAXWORD| characters, and |getword| places the next word in
the input in |buf| and returns its length, or |EOF| at the end of file.
|MAXWORD| is defined to be 101 to allow room for a terminating null
character. |addword| adds the word in |buf| to the table of words,
and |printwords(k)| prints the words with the $k$ largest frequencies.

\section

Dealing with the argument $k$ is straightforward:
\beginprogram
%include common.c arguments
	...
}
\endprogram
|atoi| and |getenv| are standard C library functions [\UPM, \S3] for converting
an ASCII string to an integer and getting the value of
an environment variable, respectively. Note the absence of error checking
for ill-formed integers. |progname| is used in printing error messages.

\section Reading Words.

|getword| reads the next word from the standard input.
This is accomplished by discarding characters up to the next occurrence
of a letter, then gathering up the letters into the argument buffer:
\beginprogram
%include common.c getword
\endprogram
|size| is compared with |1| to ensure that there is room for the
terminating null character. |isletter| is a macro that tests
for upper- or lower-case letters:
\beginprogram
%include common.c isletter
\endprogram
% 2.5 hours

\section Storing the Words.

The words must be stored in a table along with the number of times
they occur in the input. This table must accommodate two kinds of access:
while the input is being read, the table is ``indexed'' by words
in order to locate a word and increment its frequency count.
After the input has been read, the entries with the $k$ largest frequency
counts must be located and printed in decreasing order of those counts.

These two kinds of access are disjoint; that is, initially, all
accesses to the table are of the first kind, followed by only
accesses of the second kind. Consequently, the table representation can
be designed to facilitate the first kind of access, then {\it changed\/}
to facilitate the second.

A hash table is appropriate for indexing the table with words.
Since the size of the input is unknown, a hash table in which
collisions are resolved by chaining is used. Space for both the word
and the table entry can be allocated dynamically. The hash table itself,
|hashtable|, is an array of pointers to |word| structures:
\beginprogram
%include common.c hashtable
\endprogram
The bounds of |hashtable| are 0 to $2^n-1$, where $n$ is 12 here.
Using a power of 2 facilitates rapid computation of the index
into |hashtable| given a hash number: If |h| is a hash number,
the index is |h&HASHSIZE|.

\section

|hashtable| is initialized in |main|:
\beginprogram
%include common.c initialize sed 's/^	//' | prog | xref
\endprogram

\section

|addword(buf)| adds the null-terminated string in |buf| to |hashtable|,
if necessary, and increments its |count| field. To compute the index
into |hashtable|, the contents of |buf| must be ``hashed'' to yield
a hash number |h|, from which the index is computed as described above.
A simple yet effective hash function (suggested
by Hans Boehm of Rice University) is map each character in |buf|
into a random number and sum the random numbers.
This function also yields the length of the word,
which is needed to add new words to the table.
Putting this all together produces |addword|:
\beginprogram
%include common.c addword
\endprogram
|scatter| is a 128-word table containing the first 128 random
numbers returned by C library function |random|.
|strcmp| is a C library function that
returns 0 if its two arguments point to identical strings,
and |strcpy| is a C library function that copies the characters in its
second argument into its first.

\section

|alloc(n, size)| allocates space for |n| contiguous objects of
|size| bytes each:
\beginprogram
%include alloc.c alloc
\endprogram
|calloc| is a C library function that does the actual allocation
and clears the allocated space.
|alloc|'s primary purpose is to catch allocation failures,
indicated by a |NULL| value returned by |calloc|.
Many C programmers erroneously assume that |calloc| (and |malloc|)
cannot fail. On machines like the VAX, allocation rarely fails, but
on smaller machines, failure is common.
|error| prints the concatentation of its two arguments as an error
message and terminates execution; it's described on page 207 of
{\sl The UNIX Programming Environment\/} by B.~W.\ Kernighan
and R.~Pike (Prentice-Hall, 1984).

\section Printing the Words.

As suggested in the outline for |main|, given above,
|printwords(k)| prints the desired output.
To print the $k$ most common words as specified, |printwords| must
sort the contents of table in decreasing order of the |count| values,
and print the first $k$ entries. Since the frequencies range between
1 and $N$, where $N$ is the maximum frequency, sorting them can be
accomplished in time proportional to $N$ (assuming
everything fits into memory) by allocating an array of pointers to
|word|s that is indexed by the frequency of occurrence. Each element
in the array points to the list of |word|s with the same |count| values, 
that is, |list[i]| points to the list of |word|s with
|count| fields equal to |i|.
\beginprogram
%include common.c printwords sed /HASHSTAT/,/endif/d | prog | xref

\endprogram
|max| is the largest frequency count, which dictates the size of |list|.
% 2 hours

\section Performance.

Bentley did not give specific performance criteria for |common|,
but he did say that ``a user should be able to find
the 100 most frequent words in a twenty-page technical paper without
undue emotional trauma''. To test |common|,
I concatenated seven of the documents from
volume 2 of the \UPM\ from the Berkeley 4.2 \Unix\ system
to form a test file with 11,786 lines, 47,878 words (by |common|'s
definition of ``word''), 4,149 of which are unique,
and 275,516 characters.\footnote\dag{The documents
were the descriptions of |awk|, |efl|, the \Unix\ implementation,
the \Unix\ i/o system, |lex|, |sccs|, and |sed|.}

|common| with $k=0$ and this test file as input took 4.6 seconds on
a VAX 8600 running Berkeley 4.3 \Unix. By way of comparison,
consider the following program, called |charcount|:
\beginprogram
%include charcount.c main prog
\endprogram
|charcount| is about the minimum ``interesting'' program
in this class of programs, and its execution time gives a measure of the cost
of simply reading the input. With the test file as input,
|charcount| ran in 0.9 seconds. The ratio of the speed of |common| to
|charcount|, which is independent of machine dependencies such
as CPU speed and i/o costs, is 5.11.
Thus, using the implementation of |common| described above,
finding the $k$ most common words costs approximately five times as
much as just counting the characters.

% 2.5hours (includes doing timings)
\section Improvements.

To investigate the propects for improvements in the execution
speed of |common|, I profiled its execution with |gprof|
[S.~L.\ Graham, P.~B.\ Kessler, and M.~K.\ McKusick,
`An Execution Profiler for Modular Programs',
{\it Software---Practice \& Experience\/ \bf 13}(8), 671--685, Aug.\ 1983].
|gprof| takes profiling data produced by executing the program and generates
a report that details the cost of each function and its dynamic descendents.

These measurements revealed that |addword| and its descendents
accounted for 62\% of the execution time. For example, |strcmp|
was called 144,219 times and accounted for 21\% of the {\it total\/}
execution time. |strcmp| was the most frequently called function.
|getword| accounted for 32\% of the execution time,
and the other functions accounted for the remaining 6\%.

\section

The cost of |strcmp| can be reduced two ways: doing fewer comparisons
and putting the code in-line instead of calling a function.
To do the string comparison in-line,
the |if| statement in |addword| in which |strcmp| is called is replaced by
\beginprogram
%include common.c in-line-strcmp sed 's/^		//'|prog|xref +buf
		return;
		}
\endprogram
and the remainder of |addword| is revised accordingly.
This changed reduced the running time by 10.8\% to
4.56 |charcount|s (4.1 secs.).

The number of string comparisons can be reducing by storing
additional information with each word that is checked before
the string comparison is undertaken. For example, the hash number for
each word can be stored in a |hash| field and only those
words for which |wp->hash| is equal to |h| are actually compared to |buf|.
I tried this improvement and it {\it increased\/} the running time
to 5 |charcount|s. I also tried storing and comparing the lengths
instead of the hash numbers and the result was the same.

\section

The test input has 4,149 different words, which is slightly
larger than the size of |hashtable|, which is 4096.
With a hash table size of 512 (|HASHSIZE| equal to |0777|),
and the improvements described above,
the running time increased to 5.56 |charcount|s (5 secs.).
|gprof| revealed that 66\% of time was spent in |addword|,
29\% in |getword| and 5\% elsewhere.

The time spent searching the hash chains would be reduced
if the most common words were near the front of the chains.
This effect can be accomplished by using the ``move-to-front''
heuristic: each time a word is found, it is moved to the front
of its hash chain. This heuristic can be incorporated into |addword|
by adding a pointer that ``follows'' |wp| down the chain:
\beginprogram
%include common.c fast-addword
\endprogram
This change reduced the running time with a hash table size of 512
to 4.56 |charcount|s (4.1 secs.)---\kern.5pt equal to that of the time
for a hash table size of 4096 without the heuristic.
Using a hash table size of 4096 {\it and\/} the move-to-front heuristic,
the running time was 4.5 |charcount|s (4 secs.).
This last measurement verifies that the heuristic doesn't impair
performance when the size of the input is less than the hash table size,
which isn't obvious from the code.
For other applications of the move-to-front heuristic, see
J.~L.\ Bentley, D.~D.\ Sleator, R.~E.\ Tarjan, and V.~K.\ Wei,
`A Locally Adaptive Data Compression Scheme',
{\it Communications of the ACM\/ \bf 29}(4),
320--330, Apr.\ 1986, and the references therein.

\section

Identifying the common words in a 47,878-word file in 4.5 |charcount|s
seemed fast enough to avoid ``undue trauma'', so it appeared
that I was at the point of diminshing returns. Nevertheless,
I wondered if the macros for testing character classes, which
are available in a standard header file on \Unix, were significantly
faster than the |isletter| macro given above. These standard macros
use table lookup and bit testing, which could be faster than
the explicit comparisons used in |isletter|.

This change reduced the running time by 8\% to 4.1 |charcount|s (3.7 secs.).
I made the change by including the standard header file and by defining
|isletter| to be |isalpha|. On \Unix\ systems where |EOF| is not
a valid argument to the |isalpha|, |isletter| should be defined by
\beginprogram
\#define isletter(c) (c != EOF && isalpha(c))
\endprogram
I tried both definitions and the timings were the same
despite the redundant test for |EOF| in |getword| introduced
by the latter definition.

|gprof| indicated that in this {\it final\/} version of |common|,
|addword| and its descendents took 54\% of the time, |getword| took 39\%,
and everything else took the remaining 7\%.
On behalf of |addword|, |alloc| and its descendents accounted
for 11\% of the time, so allocation accounts for about
20\% of the cost of |addword|. By pre-allocating some space
at compile-time, this cost could probably be reduced by half,
but this change would yield only a 5\% speed up, so it wasn't attempted.

The changes made to improve |common|'s performance were
made as {\it additions\/} to the program and conditional
compilation is used to select the ``fast'' version. Thus, both
the program and this report describe not only the initial
program but also traces its evolution.

% 2 hours
\section Development Notes.

The initial version of |common| took about 2.5 hours. This time included
writing the program and the documentation up to the section
``Storing the Words'', and these activities were done concurrently.
I also found and fixed a bug in the |loom| system and revised
some of the \TeX\ macros used to typeset |loom|'s output.

My initial version of the program sorted the words by
making |list| (in |printwords|) an array of pointers
to the words and calling the C library function |qsort| to sort them.
Doing so involved writing a function to compare two elements in |list|.
Finishing the program and the accompanying description took another 2 hours.

This version ran in 11.22 |charcount|s (10.1 secs.) and I spent another
2.5 hours making measurements and improvements.
The general-purpose |qsort| function, which
calls a function for every comparison, accounted for over 50\% of the
execution time. Replacing it with a quicksort function
written specifically for sorting an array of pointers to |word|s
reduced the running time to 5.11 |charcount|s (4.6 secs.).
Except for the change to |isletter|, the improvements described above
were also applied and reduced the running time to 4.5 |charcount|s.

Chris Fraser and I were discussing the program in hopes of
avoiding the sorting altogether, perhaps by using a heap
to keep track of the $k$ largest frequencies during the first phase
of the program. Once we observed that the frequency counts were in the range
1 to $N$, Fraser pointed out the rather obvious linear-time
radix sort (with a radix of $N+1$) described above, and, indeed,
the measurements show that |printwords| accounts for only 1\% of the time.
Revising the program and its explanation, and re-running
the performance measurements took another 2 hours.
Finally, I spent another 2.5 hours proof reading,
making numerous minor editorial changes, and
installing, measuring, and documenting the |isletter| improvement.

\section

Typical |loom| usage involves the document file
and the program files, e.g.\ |common.lo| and |common.c|.
The document file contains references to fragments in the program files.
|loom| combines these into a \TeX\ input file, e.g.\ |common.tex|,
which is typeset by \TeX.

For small programs, such as |common|, the document and program
files are often combined into single file; for |common|,
both are combined into |common.c|.
C conditional compilation facilities are used to remove the
document part when |common.c| is compiled,
and |loom| processes |common.c| to form |common.tex|,
obtaining the code fragments from |common.c|.
Thus, a single file contains both the program and its
explanation, making |loom|'s usage similar to |WEB|'s.
|common.c| contains 662 lines and 24,899 characters.
\input index
\bye
#else
/* common: find the k most common words in the standard input */

#include <stdio.h>

/*#define FAST */		/* define for `fast' version */
#define MAXWORD 100+1

#ifdef FAST
#include <ctype.h>
#define isletter(c) isalpha(c)
#else
/* include isletter */
#define isletter(c) (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z')
/* end isletter */
#endif

/* include hashtable */
#define HASHSIZE 07777		/* hash table size */
struct word {
	char *word;		/* the word */
	int count;		/* frequency count */
	struct word *next;	/* link to next entry */
} *hashtable[HASHSIZE+1];
/* end hashtable */
int scatter[] = {		/* map characters to random values */
	2078917053, 143302914, 1027100827, 1953210302, 755253631,
	2002600785, 1405390230, 45248011, 1099951567, 433832350,
	2018585307, 438263339, 813528929, 1703199216, 618906479,
	573714703, 766270699, 275680090, 1510320440, 1583583926,
	1723401032, 1965443329, 1098183682, 1636505764, 980071615,
	1011597961, 643279273, 1315461275, 157584038, 1069844923,
	471560540, 89017443, 1213147837, 1498661368, 2042227746,
	1968401469, 1353778505, 1300134328, 2013649480, 306246424,
	1733966678, 1884751139, 744509763, 400011959, 1440466707,
	1363416242, 973726663, 59253759, 1639096332, 336563455,
	1642837685, 1215013716, 154523136, 593537720, 704035832,
	1134594751, 1605135681, 1347315106, 302572379, 1762719719,
	269676381, 774132919, 1851737163, 1482824219, 125310639,
	1746481261, 1303742040, 1479089144, 899131941, 1169907872,
	1785335569, 485614972, 907175364, 382361684, 885626931,
	200158423, 1745777927, 1859353594, 259412182, 1237390611,
	48433401, 1902249868, 304920680, 202956538, 348303940,
	1008956512, 1337551289, 1953439621, 208787970, 1640123668,
	1568675693, 478464352, 266772940, 1272929208, 1961288571,
	392083579, 871926821, 1117546963, 1871172724, 1771058762,
	139971187, 1509024645, 109190086, 1047146551, 1891386329,
	994817018, 1247304975, 1489680608, 706686964, 1506717157,
	579587572, 755120366, 1261483377, 884508252, 958076904,
	1609787317, 1893464764, 148144545, 1415743291, 2102252735,
	1788268214, 836935336, 433233439, 2055041154, 2109864544,
	247038362, 299641085, 834307717
	};
char *progname;

/* include arguments */
main(argc, argv)
int argc;
char *argv[];
{
	int i, k = 22;
	char *p, *getenv(), buf[MAXWORD];

	progname = argv[0];
	if (argc > 1)
		k = atoi(argv[1]);
	else if (p = getenv("PAGESIZE"))
		k = atoi(p);
/* end arguments */
/* include initialize */
	for (i = 0; i <= HASHSIZE; i++)
		hashtable[i] = NULL;
/* end initialize */
#ifdef SCATTER
	for (i = 0; i < 128; i++)
		printf(" %d,\n", scatter[i] = (int) random());
#endif
/* include mainprogram */
	while (getword(buf, MAXWORD) != EOF)
		addword(buf);
	printwords(k);
/* end mainprogram */
}

#ifndef FAST
/* addword - add buf[0..strlen(buf)-1] to the table, bump count */
addword(buf)
char *buf;
{
	unsigned int h;
	int len;
	char *s, *alloc();
	struct word *wp;

	h = 0;		/* compute hash number of buf[0..] */
	s = buf;
	for (len = 0; *s; len++)
		h += scatter[*s++];
	for (wp = hashtable[h&HASHSIZE]; wp; wp = wp->next)
		if (strcmp(wp->word, buf) == 0)
			break;
	if (wp == NULL) {	/* a new word */
		wp = (struct word *) alloc(1, sizeof *wp);
		wp->word = alloc(len + 1, sizeof(char));
		strcpy(wp->word, buf);
		wp->count = 0;
		wp->next = hashtable[h&HASHSIZE];
		hashtable[h&HASHSIZE] = wp;
		}
	wp->count++;
}

#else
/* addword - add buf[0..strlen(buf)-1] to the table, bump count */
/* include fast-addword */
addword(buf)
char *buf;
{
	unsigned int h;
	int len;
	char *s, *s1, *s2, *alloc();
	struct word *wp, **q, **t;

	h = 0;		/* compute hash number of buf[0..] */
	s = buf;
	for (len = 0; *s; len++)
		h += scatter[*s++];
	t = q = &hashtable[h&HASHSIZE];
	for (wp = *q; wp; q = &wp->next, wp = wp->next)
/* include in-line-strcmp */
		for (s1 = buf, s2 = wp->word; *s1 == *s2; s2++)
			if (*s1++ == '\0') {
				wp->count++;
/* end in-line-strcmp */
				if (wp != *t) {
					*q = wp->next;
					wp->next = *t;
					*t = wp;
					}
				return;
				}
	wp = (struct word *) alloc(1, sizeof *wp);
	wp->word = alloc(len + 1, sizeof(char));
	strcpy(wp->word, buf);
	wp->count = 1;
	*q = wp;
}
/* end fast-addword */
#endif

/* getword - read next word in stdin into buf[0..size-1] */
int getword(buf, size)
char *buf;
int size;
{
	char *p;
	int c;
	
	p = buf;
	while ((c = getchar()) != EOF)
		if (isletter(c)) {
			do {
				if (size > 1) {
					*p++ = c;
					size--;
					}
				c = getchar();
				} while (isletter(c));
			*p = '\0';
			return p - buf;
			}
	return EOF;
}

/* printwords - print the k most common words the table */
printwords(k)
int k;
{
	int i, max;
#ifdef HASHSTAT
	int n;
#endif
	struct word *wp, **list, *q;

	max = 0;
	for (i = 0; i <= HASHSIZE; i++)
		for (wp = hashtable[i]; wp; wp = wp->next)
			if (wp->count > max)
				max = wp->count;
	list = (struct word **) alloc(max + 1, sizeof wp);
	for (i = 0; i <= HASHSIZE; i++)
#ifdef HASHSTAT
		{ n = 0;
#endif
		for (wp = hashtable[i]; wp; wp = q) {
			q = wp->next;
			wp->next = list[wp->count];
			list[wp->count] = wp;
#ifdef HASHSTAT
			n++;
#endif
			}
#ifdef HASHSTAT
		fprintf(stderr, "%d %d\n", i, n); }
#endif
	for (i = max; i >= 0 && k > 0; i--)
		if ((wp = list[i]) && k-- > 0)
			for ( ; wp; wp = wp->next)
				printf("%d %s\n", wp->count, wp->word);
}

#include "alloc.c"
#include "error.c"
#endif
