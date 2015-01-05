
#include "parser.hh"
#include <iostream>
#include <algorithm>

namespace belr{

template <typename _parserElementT>
void Assignment<_parserElementT>::invoke(_parserElementT parent, const string &input){
	if (mChild){
		shared_ptr<ParserCollector<_parserElementT,_parserElementT>> cc=dynamic_pointer_cast<ParserCollector<_parserElementT,_parserElementT>>(mCollector);
		if (cc){
			cc->invoke(parent, mChild->realize(input));
		}
	}else{
		string value=input.substr(mBegin, mCount);
		shared_ptr<ParserCollector<_parserElementT,const string&>> cc1=dynamic_pointer_cast<ParserCollector<_parserElementT,const string&>>(mCollector);
		if (cc1){
			cc1->invoke(parent, value);
			return;
		}
		shared_ptr<ParserCollector<_parserElementT,const char*>> cc2=dynamic_pointer_cast<ParserCollector<_parserElementT,const char*>>(mCollector);
		if (cc2){
			cc2->invoke(parent, value.c_str());
			return;
		}
		shared_ptr<ParserCollector<_parserElementT,int>> cc3=dynamic_pointer_cast<ParserCollector<_parserElementT,int>>(mCollector);
		if (cc3){
			cc3->invoke(parent, atoi(value.c_str()));
			return;
		}
	}
}

template <typename _parserElementT>
ParserContext<_parserElementT>::ParserContext(Parser<_parserElementT> &parser) : mParser(parser){
}

template <typename _parserElementT>
shared_ptr<HandlerContext<_parserElementT>> ParserContext<_parserElementT>::_beginParse(const shared_ptr<Recognizer> &rec){
	shared_ptr<HandlerContext<_parserElementT>> ctx;
	auto it=mParser.mHandlers.find(rec->getName());
	if (it!=mParser.mHandlers.end()){
		ctx=(*it).second->createContext();
		mHandlerStack.push_back(ctx);
	}
	return ctx;
}

template <typename _parserElementT>
void ParserContext<_parserElementT>::_endParse(const shared_ptr<Recognizer> &rec, const shared_ptr<HandlerContext<_parserElementT>> &ctx, const string &input, size_t begin, size_t count){
	if (ctx){
		mHandlerStack.pop_back();
	}
	if (!mHandlerStack.empty()){
		/*assign object to parent */
		mHandlerStack.back()->setChild(rec->getName(), begin, count, ctx);
		
	}else{
		/*no parent, this is our root object*/
		mRoot=ctx;
	}
}

template <typename _parserElementT>
_parserElementT ParserContext<_parserElementT>::createRootObject(const string &input){
	 return mRoot ? mRoot->realize(input) : NULL;
}

template <typename _parserElementT>
shared_ptr<HandlerContext<_parserElementT>> ParserContext<_parserElementT>::_branch(){
	shared_ptr<HandlerContext<_parserElementT>> ret=mHandlerStack.back()->branch();
	mHandlerStack.push_back(ret);
	return ret;
}

template <typename _parserElementT>
void ParserContext<_parserElementT>::_merge(const shared_ptr<HandlerContext<_parserElementT>> &other){
	if (mHandlerStack.back()!=other){
		cerr<<"The branch being merged is not the last one of the stack !"<<endl;
		abort();
	}
	mHandlerStack.pop_back();
	return mHandlerStack.back()->merge(other);
}

template <typename _parserElementT>
void ParserContext<_parserElementT>::_removeBranch(const shared_ptr<HandlerContext<_parserElementT>> &other){
	auto it=find(mHandlerStack.rbegin(), mHandlerStack.rend(),other);
	if (it==mHandlerStack.rend()){
		cerr<<"A branch could not be found in the stack while removing it !"<<endl;
		abort();
	}else{
		advance(it,1);
		mHandlerStack.erase(it.base());
	}
}

template <typename _parserElementT>
shared_ptr<HandlerContextBase> ParserContext<_parserElementT>::beginParse(const shared_ptr<Recognizer> &rec){
	return static_pointer_cast<HandlerContext<_parserElementT>>(_beginParse(rec));
}

template <typename _parserElementT>
void ParserContext<_parserElementT>::endParse(const shared_ptr<Recognizer> &rec, const shared_ptr<HandlerContextBase> &ctx, const string &input, size_t begin, size_t count){
	_endParse(rec,static_pointer_cast<HandlerContext<_parserElementT>>(ctx), input, begin, count);
}

template <typename _parserElementT>
shared_ptr<HandlerContextBase> ParserContext<_parserElementT>::branch(){
	return static_pointer_cast<HandlerContext<_parserElementT>>(_branch());
}

template <typename _parserElementT>
void ParserContext<_parserElementT>::merge(const shared_ptr<HandlerContextBase> &other){
	_merge(static_pointer_cast<HandlerContext<_parserElementT>>(other));
}

template <typename _parserElementT>
void ParserContext<_parserElementT>::removeBranch(const shared_ptr<HandlerContextBase> &other){
	_removeBranch(static_pointer_cast<HandlerContext<_parserElementT>>(other));
}

template <typename _parserElementT>
shared_ptr<HandlerContext<_parserElementT>> ParserHandler<_parserElementT>::createContext(){
	return make_shared<HandlerContext<_parserElementT>>(this->shared_from_this());
}
	
template <typename _parserElementT>
Parser<_parserElementT>::Parser(const shared_ptr<Grammar> &grammar) : mGrammar(grammar){
	if (!mGrammar->isComplete()){
		cerr<<"Grammar not complete, aborting."<<endl;
		return;
	}
}

template <typename _parserElementT>
_parserElementT Parser<_parserElementT>::parseInput(const string &rulename, const string &input, size_t *parsed_size){
	size_t parsed;
	shared_ptr<Recognizer> rec=mGrammar->getRule(rulename);
	auto pctx=make_shared<ParserContext<_parserElementT>>(*this);
	
	parsed=rec->feed(pctx, input, 0);
	if (parsed_size) *parsed_size=parsed;
	return pctx->createRootObject(input);
}

}