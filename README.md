<h1>Linux Payment Terminal Drivers</h1>

Contains:
<ul>
<li>Printers:
	<ul>
	<li>Shtrih MINI FR K (Штрих МИНИ ФР-К) - <i>russian cash registrator with daily sessions based on RS-232 (COM port)</i></li>
	</ul>
</li>
<li>Validators:
	<ul>
		<li>CashCode MFL - <i>cash validator based on ID003(JCM) protocol and RS-232</i></li>
		<li>CashCode SM - <i>cash validator based on CC-Net protocol and RS-232</i></li>
	</ul>
</li>
</ul>

USAGE:
<ul>
<li>Operating system must be linux based (Ubuntu is better)</li>
<li>Parameters for validator:
<pre>
	 * 	1 reset
	 * 	2 ack
	 * 	3 poll
	 * 	4 btbl
	 * 	5 disbtps
	 *  6 enbtbs
	 * 	7 identif
	 * 	8 stsecr
	 Parameters for scripts:
	 *  9 s_init
	 *  10 s_poll
</pre>
</li>
<li>Output formats:
	<pre>RESULT CODE|MESSAGE CODE|FULL CODE</pre>
</li>
<li>RESULT CODES:
	<pre>
 * 80 - error (unknown mode)
 * 81 - error (wrong parameter count)
 * 82 - error (cannot open device)
 * 83 - error (no params)
 * 84 - error (cant reading config)
 * 11 - ok with responce
 * 12 - ok empty
 * 21 - write reset ok
 * 22 - write ack ok
 * 23 - write poll ok
 * 24 - write bill table ok
 * 25 - write disbtbls ok
 * 26 - write enbiltbls ok
 * 27 - write identif ok
 * 28 - write set secr ok
 * 29 - read port empty
 * 2a - read port error
 * 2b - read port success
	</pre>
</li>
</ul>