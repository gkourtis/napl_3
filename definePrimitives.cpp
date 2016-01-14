__attribute__((constructor))
void definePrimitives(){

#define Cl(a,x,b,voic) dict.define(NAPL(x),x);
		classInfoNames
#undef mcr

classInfos(classDeferred).executeFunction=[](obj o){
	for(o=o->value();o.Class()==classDeferred;o=o->value()){}
	o.execute();
};

classInfos(classClass).executeFunction=[](obj o){etP->stack().push(o);};
classInfos(classComment).executeFunction=[](obj o){};

}


#define TOS ((obj)(etP->stack().tos()))
#define PUSH(x) (etP->stack().push(x))
#define POP(x) ((obj)(etP->stack().pop(x)))

def(primitive0,)
def(drop,etP->stack().drop(1);)
def(dropall,etP->stack().dropall();)
def(depth,etP->stack().push(etP->stack().elements());)

def(nip,etP->stack().nip();)
def(tuck,etP->stack().tuck();)

// stack() operations duplicating an element should enable the readOnly flag.
def(over,etP->stack().over();)
def(pick,int i=etP->stack().popv();etP->stack().push(etP->stack().at(etP->stack().elements()-i));)
def(dup,etP->stack().dup();)

def(swap,Token t2=etP->stack().pop();Token t1=etP->stack().pop();etP->stack().push(t2);etP->stack().push(t1);)
def0(rollRight,"roll>",etP->stack().roll(1,etP->stack().popv());)
def0(rollLeft,"roll<",etP->stack().roll(0,etP->stack().popv());)

def0(intPlusMinus,"int+-",etP->stack().push(newInt(-etP->stack().pop()).datum_T<Int>());)

def0(intdivmod,"int/mod",
	Word w2=etP->stack().popvint();Word w1=etP->stack().popvint();
	div_t r=div(w1,w2);
	etP->stack().push(r.quot);etP->stack().pushint(r.rem);
);

def0(openBracket,"[",	etP->newStack(0);) // creates a new stack() and pushes it into the stack() hierarchy
def0(openStack,"_[",etP->openStack();)	 // takes the tos that should be an aggregate and pushes it into the stack() hierarcy 

def0(closeBracket,"]",etP->oldStack();) 
def(is,
	Obj value=etP->stack().pop();
	Obj name=etP->stack().pop();
	dict.define(name,value);
);

def0(justapose,"|",
	Obj o=concatenateStrings((String)etP->stack()[~2],(String)etP->stack()[~1]);
	etP->stack().drop(2);etP->stack().push(o);
);

def(slice,
	Word j=etP->stack().pop();Word i=etP->stack().pop();
	Aggregate a=(Aggregate)etP->stack().pop();
	etP->stack().push(concatenateSlices0(1,3,a.w,i,j));
);

def(bye,etP->exitInterpretLoop=true;);

def(setAutoLevel,etP->setExecutionLevel(););
def(setLevel,
	int l=etP->stack().pop();
	obj o=etP->stack().tos();
	o.executionLevel(l);
);

def(execute,etP->execute(etP->stack().pop(),true););

def(debug,etP->execute(etP->stack().pop(),true,true););
def(ifelse,
	Token t2=etP->stack().pop();Token t1=etP->stack().pop();if(etP->stack().pop())etP->execute(t1,true);else etP->execute(t2,true);
)
def(if_,
	Token t=etP->stack().pop();
		if(etP->stack().pop())
			etP->execute(t,true);
)		

def(include,
	etP->print("Including ",toString(obj(etP->stack().tos())));
	tString s(file2String(etP->stack().pop()));
	etP->interpret(s);
	etP->print(" included");
)

def(print,etP->print((obj)etP->stack().tos());etP->stack().drop(););		

def(show,std::cout<<std::endl<<toString(etP->stack().pop(),false,false)<<std::endl<<std::endl;)
	
def(newline,etP->write("\n"););
def(i,etP->stack().push(etP->iCounter);)
def0(set_i,"i->",etP->iCounter=etP->stack().popv();)

def0(loop_,"loop",
	Token t=etP->stack().pop();
	for(;;) {etP->execute(t,true);if(!etP->mustContinue())break;} 	
)

// attention, because the iCounter value may be used after the loop when !mustContinue shouldn't
// be inside the loop conditions with the incrementing operator because wrong results may be given
// if the checks are done in the wrong order

def0(timesQuestion,"times?",
	Token t=etP->stack().pop();
	backup<Token> b((Token&)etP->iCounter,etP->stack().popv());
	for(;--etP->iCounter>=0;)                   {etP->execute(t,true);if(!etP->mustContinue())break;}
	etP->stack().push(etP->iCounter);
)

def0(forQ,"for?",
	Token t=etP->stack().pop();
		int step=etP->stack().popv(); int i2=etP->stack().popv();
	backup<Token> b((Token&)etP->iCounter,etP->stack().popv());
	if     (step>0) 
		for( ; etP->iCounter<i2 ; etP->iCounter+=step) {etP->execute(t,true);if(!etP->mustContinue())break;}
	else if(step<0)
		for( ;etP->iCounter+=step,etP->iCounter>=i2; ) {etP->execute(t,true);if(!etP->mustContinue())break;}
	else
		std::wcout<<L"problem with step == 0 in for";
	etP->stack().push(etP->iCounter);
);

defAuto("?.exit",int n=etP->stack().popv();bool b=etP->stack().popv();etP->exitSec(b,n);)
defAuto("?exit",etP->exitSec(etP->stack().popv(),1);)
defAuto("?cont",etP->exitSec(etP->stack().popv()==0,1);)
	
defAuto("until",etP->exitSec(etP->stack().popv(),2);)
defAuto("while",etP->exitSec(etP->stack().popv()==0,2);)
defAuto(".exit",etP->exitSec(true,etP->stack().popv());)
defAuto("exit",etP->exitSec(true,1);)

defAuto("compact",stopETs.lock();compact(M);stopETs.unlock();)
	
defAuto("checkCompact",checkCompact(M);)
	
defAuto("~",etP->counter++;)

defAuto("?~",etP->counter=etP->counter.l+etP->stack().pop();)

defAuto("elc",etP->stack().push(etP->counter.l);)
defAuto("->",
	obj o1=etP->stack().pop();
	obj o=etP->stack().pop();
	o.cloneIn(o1);
)

defAuto("<-",
		obj o{etP->stack().pop()}; obj o1{etP->stack().pop()};
		o.cloneIn(o1);
)	
	
defAuto("??",etP->showStack();std::cout<<std::endl;)

defAuto("noop",);

defAuto(">|>",etP->interStackMove();)
defAuto("dup>|>",etP->interStackDup();)
defAuto("isName",etP->interStackDup();)
defAuto("int2double",int i=etP->stack().pop();etP->stack().push(newDouble((double)i));)

defAuto("foreach",obj action=etP->stack().pop();Aggregate a=(Aggregate)etP->stack().pop();etP->foreach(a,action,false);)
defAuto("foreachRev",obj action=etP->stack().pop();Aggregate a=(Aggregate)etP->stack().pop();etP->foreach(a,action,true );)
defAuto("setClass",obj class_t=etP->stack().pop();obj o=etP->stack().tos();o.Class(class_t);)
defAuto(".class",PUSH(TOS.Class());)
defAuto("msecs",etP->stack().push(newInt(msecs()));)

defAuto("in",obj o=etP->stack().pop();Aggregate a=(Aggregate)etP->stack().pop();etP->stack().push(newInt(a.in(o)));)
	/*
	definefunc(L"at",[](){
		obj& o=O(etP->stack().pop());o.data<int>()
		int i=stack().popv();aggregate a(etP->stack().pop());stack().push(a.at(i));});
	*/
defAuto("elements",Aggregate a=(Aggregate)etP->stack().pop();etP->stack().push(newInt(a.elements()));)
defAuto("tab",std::wcout<<L"\t";)
	
defAuto("localsSet",localsSet();)


defAuto("dictShowAll",dictShow(true);)
defAuto("dictShow",dictShow();)

defAuto("asFields",((obj)etP->stack().tos())->fields=true;)

defAuto("hideFields",((obj)etP->stack().tos())->hideFields=true;)
defAuto("showFields",((obj)etP->stack().tos())->hideFields=false;)

defAuto("cwd",
		char* d=getcwd(nullptr,0);
			etP->stack().push(::newString(d));
		free(d);
	)

defAuto("garbageCollect",stopETs.lock();garbageCollect(M);stopETs.unlock();)

defAuto("allPos",etP->stack().push(etP->allPos(etP->stack()[~1]));)
defAuto("anything",
	tAggregateOrdered o{etP->stack().pop()};
	etP->stack().push(etP->anything(etP->stack()[~1],o));
)

defAuto("match",
		Obj pat=etP->stack().pop();Obj positions=etP->stack().pop();
		etP->stack().push(etP->match((AggregateOrdered)etP->stack()[~1],positions,pat));
)

defAuto("match0+",etP->matchFor_(0,uWordMax);)
defAuto("match01",etP->matchFor_(0,1);)
defAuto("match1+",etP->matchFor_(1,uWordMax);)
defAuto("matchFor",uWord n2=etP->stack().pop();uWord n1=etP->stack().pop();etP->matchFor_(n1,n2);)
defAuto("matchEnd",
		AggregateOrdered positions=(Aggregate)etP->stack().pop();
		etP->stack().push(etP->matchEnd(etP->stack()[~1],positions));
	)
	
defAuto("compilerConf",compilerConf();)
	
defAuto("ref",etP->stack().push((obj)newDeferred(etP->stack().pop()));)

defAuto("abandon",etP->exitInterpretLoop=abandon=true;)

//defAuto("setExcludeFirst",obj{etP->stack().tos()}->excludeFirst=true;)

//defAuto("asPrototype",((obj)etP->stack().tos()).Class(classPrototype);)

defAuto("$",etP->stack().push(toString(etP->stack().tos(),false));)

#define binaryOper(name,operation) \
defAuto(#name,Token t2=etP->stack().popv();Token t1=etP->stack().popv();int r=operation(t1,t2);etP->stack().push(r<<1>>1);)
#define binaryOperInt(name,operation) binaryOper(name,std::operation<Token>())
#define d(name,op) binaryOperInt(name,op)

d(int+,plus) d(int-,minus) d(int*,multiplies) d(int/,divides) d(intMod,modulus) 
d(int>,greater) d(int>=,greater_equal) d(int<,less) d(int<=,less_equal) d(int=,equal_to) d(int<>,not_equal_to)
d(intBitAnd,bit_and) d(intBitOr,bit_or) d(intBitXor,bit_xor) binaryOper(intBit<=,bitLessEqual)

#define operFloat(name,xxx) \
defAuto(#name,\
	Obj o2=etP->stack().pop();Obj o1=etP->stack().pop();\
	double r= std::xxx(o1.datum_T<double>(),o2.datum_T<double>());etP->stack().push(newDouble(r));\
)

operFloat(double+,plus<double>())
operFloat(double-,minus<double>())
operFloat(double*,multiplies<double>())
operFloat(double/,divides<double>())
operFloat(double^,pow)

def00Level(curlyOpenT,autoname,".{",
	etP->curlyOpen(etP->stack().pop());
	,std::numeric_limits<executionLevel_t>::max()
)
	
def00Level(curlyCloseT,autoname,".}",
		etP->curlyClose(etP->stack().pop());
		,std::numeric_limits<executionLevel_t>::max()
)

def00(localsUnsetT,autoname,"localsUnset",localsUnset();)

def00Level(lastToken,autoname,"~~",
	etP->counter++;
	,2
)


