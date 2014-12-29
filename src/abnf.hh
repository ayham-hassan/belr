#include "belr.hh"

namespace belr{


/*
 * Core definitions of ABNF
**/

class CoreRules : public Grammar{
public:
	CoreRules();
private:
	void alpha();
	void bit();
	void char_();
	void cr();
	void lf();
	void crlf();
	void ctl();
	void digit();
	void dquote();
	void hexdig();
	void htab();
	void lwsp();
	void octet();
	void sp();
	void vchar();
	void wsp();
};



class ABNFGrammar : public Grammar{
public:
	ABNFGrammar();
private:
	void comment();
	void c_nl();
	void c_wsp();
	void rulename();
	void repeat();
	void defined_as();
	void rulelist();
	void rule();
	void elements();
	void alternation();
	void concatenation();
	void repetition();
	void element();
	void group();
	void option();
	void char_val();
	void num_val();
	void prose_val();
	void bin_val();
	void dec_val();
	void hex_val();
};


}//end of namespace
