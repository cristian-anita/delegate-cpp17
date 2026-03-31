// MIT License: https://spdx.org/licenses/MIT.html
// 
// Copyright (c) 2026 Cristian ANITA <cristian_anita@yahoo.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.


#include "delegate.hpp"

#include <string>
#include <cctype>
#include <cstddef>
#include <stdexcept>
#include <iostream>


#define BRICXX_CHECK(expr)	\
	do { \
		if (!(expr)) { \
			throw runtime_error(#expr); \
		} \
	} while (false)


using namespace bricxx;
using namespace std;


void TestHeaderDoc();
void TestDelegate();
void TestConstDelegate();
void TestNoexceptDelegate();
void TestConstNoexceptDelegate();


long FunctionAddLong(long x, long y);

class FunctorAddLong {
public:
	long operator()(long x, long y);
	
	int GetNumCalls() const;
private:
	int numCalls_ = 0;
};

class ClassAddLong {
public:
	long Add(long x, long y);
	
	int GetNumCalls() const;
private:
	int numCalls_ = 0;
};


class ConstFunctorAddLong {
public:
	long operator()(long x, long y) const;
};

class ConstClassAddLong {
public:
	long Add(long x, long y) const;
};


long NoexceptFunctionAddLong(long x, long y) noexcept;

class NoexceptFunctorAddLong {
public:
	long operator()(long x, long y) noexcept;
	
	int GetNumCalls() const;
private:
	int numCalls_ = 0;
};

class NoexceptClassAddLong {
public:
	long Add(long x, long y) noexcept;
	
	int GetNumCalls() const;
private:
	int numCalls_ = 0;
};


class ConstNoexceptFunctorAddLong {
public:
	long operator()(long x, long y) const noexcept;
};

class ConstNoexceptClassAddLong {
public:
	long Add(long x, long y) const noexcept;
};


int FunctionAddInt(int x, int y);

class FunctorAddInt {
public:
	int operator()(int x, int y);
	
	int GetNumCalls() const;
private:
	int numCalls_ = 0;
};

class ClassAddInt {
public:
	int Add(int x, int y);
	
	int GetNumCalls() const;
private:
	int numCalls_ = 0;
};


class ConstFunctorAddInt {
public:
	int operator()(int x, int y) const;
};

class ConstClassAddInt {
public:
	int Add(int x, int y) const;
};


int NoexceptFunctionAddInt(int x, int y) noexcept;

class NoexceptFunctorAddInt {
public:
	int operator()(int x, int y) noexcept;
	
	int GetNumCalls() const;
private:
	int numCalls_ = 0;
};

class NoexceptClassAddInt {
public:
	int Add(int x, int y) noexcept;
	
	int GetNumCalls() const;
private:
	int numCalls_ = 0;
};


class ConstNoexceptFunctorAddInt {
public:
	int operator()(int x, int y) const noexcept;
};

class ConstNoexceptClassAddInt {
public:
	int Add(int x, int y) const noexcept;
};


void TestHeaderDoc() {
	// (non-member) function
	Delegate<int (int)> functionDelegate;
	functionDelegate.Bind<&::islower>();
	if (!functionDelegate.IsBindedTo<&::islower>()) throw runtime_error("function test failed");
	if (!functionDelegate('a')) throw runtime_error("function test failed");
	
	// lambda/functor
	int numCalls = 0;
	auto lambda = [&numCalls](int x, int y) mutable -> bool {
		++numCalls;
		return x > y;
	};
	Delegate<bool (int, int)> lambdaDelegate;
	lambdaDelegate.Bind(lambda);
	if (!lambdaDelegate.IsBindedTo(lambda)) throw runtime_error("lambda test failed");
	if (!lambdaDelegate(2, 1)) throw runtime_error("lambda test failed");
	if (numCalls != 1) throw runtime_error("lambda test failed");
	
	// method (member function)
	string hello = "Hello, World!";
	Delegate<void ()> methodDelegate;
	methodDelegate.Bind<&string::clear>(hello);
	methodDelegate();
	if (!hello.empty()) throw runtime_error("method test failed");
	
	// comparison
	Delegate<int (int)> functionDelegate1;
	functionDelegate1.Bind<&::isupper>();
	if (functionDelegate == functionDelegate1) throw runtime_error("comparison test failed");
	functionDelegate1.Bind<&::islower>();
	if (functionDelegate != functionDelegate1) throw runtime_error("comparison test failed");
	
	// const-correctness
	Delegate<void () const> constDelegate;
	//constDelegate.Bind<&string::clear>(hello);	// OK, does not compile
	(void)constDelegate;
	
	// noexcept-correctness
	Delegate<void (char) noexcept> noexceptDelegate;
	//noexceptDelegate.Bind<&string::push_back>(hello);	// OK, does not compile
	(void)noexceptDelegate;
	
	// unbind
	functionDelegate.Unbind();
	if (functionDelegate.IsBinded()) throw runtime_error("unbind test failed");
	
	// BadDelegateCall
	bool caught = false;
	try {
		(void)functionDelegate('a');
	}
	catch (const BadDelegateCall&) {
		caught = true;
	}
	if (!caught) throw runtime_error("BadDelegateCall test failed");
	
	// compatible signatures
	// long atol(const char* str);
	// int atoi(const char* str);
	Delegate<long (const char*)> delegate;
	delegate.Bind<&::std::atoi>();
	if (!delegate.IsBindedTo<&::std::atoi>()) throw runtime_error("compatible signatures test failed");
	if (delegate("10") != 10) throw runtime_error("compatible signatures test failed");
}

void TestDelegate() {
	// 1 constructor, BadDelegateCall
	{
		Delegate<void ()> delegate;
		BRICXX_CHECK(!delegate.IsBinded());
		
		bool caught = false;
		try {
			delegate();
		}
		catch (const BadDelegateCall&) {
			caught = true;
		}
		BRICXX_CHECK(caught);
	}
	
	
	//************************************************************
	// 2 bind/unbind
	
	// 2.1 function
	{
		Delegate<long (long, long)> delegate;
		delegate.Bind<&FunctionAddLong>();
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo<&FunctionAddLong>());
		BRICXX_CHECK(delegate(1L, 2L) == 3L);
		delegate.Unbind();
		BRICXX_CHECK(!delegate.IsBinded());
	}
	
	// 2.2 functor
	{
		FunctorAddLong functorAddLong;
		
		Delegate<long (long, long)> delegate;
		delegate.Bind(functorAddLong);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(functorAddLong));
		BRICXX_CHECK(delegate(1L, 2L) == 3L);
		BRICXX_CHECK(functorAddLong.GetNumCalls() == 1);
		delegate.Unbind();
		BRICXX_CHECK(!delegate.IsBinded());
	}
	
	// 2.3 lambda
	{
		int numCalls = 0;
		auto lambdaAddLong = [&numCalls](long x, long y) mutable -> long {
			++numCalls;
			return x + y;
		};
		
		Delegate<long (long, long)> delegate;
		delegate.Bind(lambdaAddLong);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(lambdaAddLong));
		BRICXX_CHECK(delegate(1L, 2L) == 3L);
		BRICXX_CHECK(numCalls == 1);
		delegate.Unbind();
		BRICXX_CHECK(!delegate.IsBinded());
	}
	
	// 2.4 member
	{
		ClassAddLong classAddLong;
		
		Delegate<long (long, long)> delegate;
		delegate.Bind<&ClassAddLong::Add>(classAddLong);
		BRICXX_CHECK(delegate.IsBinded());
		bool classAddLongIsBindedTo = delegate.IsBindedTo<&ClassAddLong::Add>(classAddLong);
		BRICXX_CHECK(classAddLongIsBindedTo);
		BRICXX_CHECK(delegate(1L, 2L) == 3L);
		BRICXX_CHECK(classAddLong.GetNumCalls() == 1);
		delegate.Unbind();
		BRICXX_CHECK(!delegate.IsBinded());
	}
	
	
	//************************************************************
	// 3 copy constructor/assignment, equality
	
	// 3.1 function
	{
		Delegate<long (long, long)> delegate;
		delegate.Bind<&FunctionAddLong>();
		
		Delegate<long (long, long)> delegate2 = delegate;
		BRICXX_CHECK(delegate2.IsBinded());
		BRICXX_CHECK(delegate2.IsBindedTo<&FunctionAddLong>());
		BRICXX_CHECK(delegate2(1L, 2L) == 3L);
		
		Delegate<long (long, long)> delegate3;
		delegate3 = delegate;
		BRICXX_CHECK(delegate3.IsBinded());
		BRICXX_CHECK(delegate3.IsBindedTo<&FunctionAddLong>());
		BRICXX_CHECK(delegate3(2L, 3L) == 5L);
		
		BRICXX_CHECK(delegate2 == delegate3);
		delegate3.Unbind();
		BRICXX_CHECK(delegate2 != delegate3);
	}
	
	// 3.2 functor
	{
		FunctorAddLong functorAddLong;
		
		Delegate<long (long, long)> delegate;
		delegate.Bind(functorAddLong);
		
		Delegate<long (long, long)> delegate2 = delegate;
		BRICXX_CHECK(delegate2.IsBinded());
		BRICXX_CHECK(delegate2.IsBindedTo(functorAddLong));
		BRICXX_CHECK(delegate2(1L, 2L) == 3L);
		BRICXX_CHECK(functorAddLong.GetNumCalls() == 1);
		
		Delegate<long (long, long)> delegate3;
		delegate3 = delegate;
		BRICXX_CHECK(delegate3.IsBinded());
		BRICXX_CHECK(delegate3.IsBindedTo(functorAddLong));
		BRICXX_CHECK(delegate3(2L, 3L) == 5L);
		BRICXX_CHECK(functorAddLong.GetNumCalls() == 2);
		
		BRICXX_CHECK(delegate2 == delegate3);
		delegate3.Unbind();
		BRICXX_CHECK(delegate2 != delegate3);
	}
	
	// 3.3 lambda
	{
		int numCalls = 0;
		auto lambdaAddLong = [&numCalls](long x, long y) mutable -> long {
			++numCalls;
			return x + y;
		};
		
		Delegate<long (long, long)> delegate;
		delegate.Bind(lambdaAddLong);
		
		Delegate<long (long, long)> delegate2 = delegate;
		BRICXX_CHECK(delegate2.IsBinded());
		BRICXX_CHECK(delegate2.IsBindedTo(lambdaAddLong));
		BRICXX_CHECK(delegate2(1L, 2L) == 3L);
		BRICXX_CHECK(numCalls == 1);
		
		Delegate<long (long, long)> delegate3;
		delegate3 = delegate;
		BRICXX_CHECK(delegate3.IsBinded());
		BRICXX_CHECK(delegate3.IsBindedTo(lambdaAddLong));
		BRICXX_CHECK(delegate3(2L, 3L) == 5L);
		BRICXX_CHECK(numCalls == 2);
		
		BRICXX_CHECK(delegate2 == delegate3);
		delegate3.Unbind();
		BRICXX_CHECK(delegate2 != delegate3);
	}
	
	// 3.4 member
	{
		ClassAddLong classAddLong;
		
		Delegate<long (long, long)> delegate;
		delegate.Bind<&ClassAddLong::Add>(classAddLong);
		
		Delegate<long (long, long)> delegate2 = delegate;
		BRICXX_CHECK(delegate2.IsBinded());
		bool classAddLongIsBindedTo = delegate2.IsBindedTo<&ClassAddLong::Add>(classAddLong);
		BRICXX_CHECK(classAddLongIsBindedTo);
		BRICXX_CHECK(delegate2(1L, 2L) == 3L);
		BRICXX_CHECK(classAddLong.GetNumCalls() == 1);
		
		Delegate<long (long, long)> delegate3;
		delegate3 = delegate;
		BRICXX_CHECK(delegate3.IsBinded());
		classAddLongIsBindedTo = delegate3.IsBindedTo<&ClassAddLong::Add>(classAddLong);
		BRICXX_CHECK(classAddLongIsBindedTo);
		BRICXX_CHECK(delegate3(2L, 3L) == 5L);
		BRICXX_CHECK(classAddLong.GetNumCalls() == 2);
		
		BRICXX_CHECK(delegate2 == delegate3);
		delegate3.Unbind();
		BRICXX_CHECK(delegate2 != delegate3);
	}
	
	
	//************************************************************
	// 4 compatible signatures
	
	// 4.1 function
	{
		Delegate<long (long, long)> delegate;
		delegate.Bind<&FunctionAddInt>();
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo<&FunctionAddInt>());
		BRICXX_CHECK(delegate(1, 2) == 3);
	}
	
	// 4.2 functor
	{
		FunctorAddInt functorAddInt;
		
		Delegate<long (long, long)> delegate;
		delegate.Bind(functorAddInt);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(functorAddInt));
		BRICXX_CHECK(delegate(1, 2) == 3);
		BRICXX_CHECK(functorAddInt.GetNumCalls() == 1);
	}
	
	// 4.3 lambda
	{
		int numCalls = 0;
		auto lambdaAddInt = [&numCalls](int x, int y) mutable -> int {
			++numCalls;
			return x + y;
		};
		
		Delegate<long (long, long)> delegate;
		delegate.Bind(lambdaAddInt);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(lambdaAddInt));
		BRICXX_CHECK(delegate(1, 2) == 3);
		BRICXX_CHECK(numCalls == 1);
	}
	
	// 4.4 member
	{
		ClassAddInt classAddInt;
		
		Delegate<long (long, long)> delegate;
		delegate.Bind<&ClassAddInt::Add>(classAddInt);
		BRICXX_CHECK(delegate.IsBinded());
		bool classAddIntIsBindedTo = delegate.IsBindedTo<&ClassAddInt::Add>(classAddInt);
		BRICXX_CHECK(classAddIntIsBindedTo);
		BRICXX_CHECK(delegate(1, 2) == 3);
		BRICXX_CHECK(classAddInt.GetNumCalls() == 1);
	}
}

void TestConstDelegate() {
	// 1 constructor, BadDelegateCall
	{
		Delegate<void () const> delegate;
		BRICXX_CHECK(!delegate.IsBinded());
		
		bool caught = false;
		try {
			delegate();
		}
		catch (const BadDelegateCall&) {
			caught = true;
		}
		BRICXX_CHECK(caught);
	}
	
	
	//************************************************************
	// 2 bind/unbind
	
	// 2.1 functor
	{
		ConstFunctorAddLong constFunctorAddLong;
		
		Delegate<long (long, long) const> delegate;
		delegate.Bind(constFunctorAddLong);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(constFunctorAddLong));
		BRICXX_CHECK(delegate(1L, 2L) == 3L);
		delegate.Unbind();
		BRICXX_CHECK(!delegate.IsBinded());
	}
	
	// 2.2 lambda
	{
		auto constLambdaAddLong = [](long x, long y) -> long {
			return x + y;
		};
		
		Delegate<long (long, long) const> delegate;
		delegate.Bind(constLambdaAddLong);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(constLambdaAddLong));
		BRICXX_CHECK(delegate(1L, 2L) == 3L);
		delegate.Unbind();
		BRICXX_CHECK(!delegate.IsBinded());
	}
	
	// 2.3 member
	{
		ConstClassAddLong constClassAddLong;
		
		Delegate<long (long, long) const> delegate;
		delegate.Bind<&ConstClassAddLong::Add>(constClassAddLong);
		BRICXX_CHECK(delegate.IsBinded());
		bool constClassAddLongIsBindedTo = delegate.IsBindedTo<&ConstClassAddLong::Add>(constClassAddLong);
		BRICXX_CHECK(constClassAddLongIsBindedTo);
		BRICXX_CHECK(delegate(1L, 2L) == 3L);
		delegate.Unbind();
		BRICXX_CHECK(!delegate.IsBinded());
	}
	
	
	//************************************************************
	// 3 copy constructor/assignment, equality
	
	// 3.1 functor
	{
		ConstFunctorAddLong constFunctorAddLong;
		
		Delegate<long (long, long) const> delegate;
		delegate.Bind(constFunctorAddLong);
		
		Delegate<long (long, long) const> delegate2 = delegate;
		BRICXX_CHECK(delegate2.IsBinded());
		BRICXX_CHECK(delegate2.IsBindedTo(constFunctorAddLong));
		BRICXX_CHECK(delegate2(1L, 2L) == 3L);
		
		Delegate<long (long, long) const> delegate3;
		delegate3 = delegate;
		BRICXX_CHECK(delegate3.IsBinded());
		BRICXX_CHECK(delegate3.IsBindedTo(constFunctorAddLong));
		BRICXX_CHECK(delegate3(2L, 3L) == 5L);
		
		BRICXX_CHECK(delegate2 == delegate3);
		delegate3.Unbind();
		BRICXX_CHECK(delegate2 != delegate3);
	}
	
	// 3.2 lambda
	{
		auto constLambdaAddLong = [](long x, long y) -> long {
			return x + y;
		};
		
		Delegate<long (long, long) const> delegate;
		delegate.Bind(constLambdaAddLong);
		
		Delegate<long (long, long) const> delegate2 = delegate;
		BRICXX_CHECK(delegate2.IsBinded());
		BRICXX_CHECK(delegate2.IsBindedTo(constLambdaAddLong));
		BRICXX_CHECK(delegate2(1L, 2L) == 3L);
		
		Delegate<long (long, long) const> delegate3;
		delegate3 = delegate;
		BRICXX_CHECK(delegate3.IsBinded());
		BRICXX_CHECK(delegate3.IsBindedTo(constLambdaAddLong));
		BRICXX_CHECK(delegate3(2L, 3L) == 5L);
		
		BRICXX_CHECK(delegate2 == delegate3);
		delegate3.Unbind();
		BRICXX_CHECK(delegate2 != delegate3);
	}
	
	// 3.3 member
	{
		ConstClassAddLong constClassAddLong;
		
		Delegate<long (long, long) const> delegate;
		delegate.Bind<&ConstClassAddLong::Add>(constClassAddLong);
		
		Delegate<long (long, long) const> delegate2 = delegate;
		BRICXX_CHECK(delegate2.IsBinded());
		bool constClassAddLongIsBindedTo = delegate2.IsBindedTo<&ConstClassAddLong::Add>(constClassAddLong);
		BRICXX_CHECK(constClassAddLongIsBindedTo);
		BRICXX_CHECK(delegate2(1L, 2L) == 3L);
		
		Delegate<long (long, long) const> delegate3;
		delegate3 = delegate;
		BRICXX_CHECK(delegate3.IsBinded());
		constClassAddLongIsBindedTo = delegate3.IsBindedTo<&ConstClassAddLong::Add>(constClassAddLong);
		BRICXX_CHECK(constClassAddLongIsBindedTo);
		BRICXX_CHECK(delegate3(2L, 3L) == 5L);
		
		BRICXX_CHECK(delegate2 == delegate3);
		delegate3.Unbind();
		BRICXX_CHECK(delegate2 != delegate3);
	}
	
	
	//************************************************************
	// 4 const-correctness
	
	// 4.1 functor
	{
		FunctorAddLong functorAddLong;
		
		Delegate<long (long, long) const> delegate;
		//delegate.Bind(functorAddLong);	// OK, does not compile
		(void)functorAddLong;
		(void)delegate;
	}
	
	// 4.2 lambda
	{
		int numCalls = 0;
		auto lambdaAddLong = [&numCalls](long x, long y) mutable -> long {
			++numCalls;
			return x + y;
		};
		
		Delegate<long (long, long) const> delegate;
		//delegate.Bind(lambdaAddLong);	// OK, does not compile
		(void)lambdaAddLong;
		(void)delegate;
	}
	
	// 4.3 member
	{
		ClassAddLong classAddLong;
		
		Delegate<long (long, long) const> delegate;
		//delegate.Bind<&ClassAddLong::Add>(classAddLong);	// OK, does not compile
		(void)classAddLong;
		(void)delegate;
	}
	
	
	//************************************************************
	// 5 compatible signatures
	
	// 5.1 functor
	{
		ConstFunctorAddInt constFunctorAddInt;
		
		Delegate<long (long, long) const> delegate;
		delegate.Bind(constFunctorAddInt);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(constFunctorAddInt));
		BRICXX_CHECK(delegate(1, 2) == 3);
	}
	
	// 5.2 lambda
	{
		auto constLambdaAddInt = [](int x, int y) -> int {
			return x + y;
		};
		
		Delegate<long (long, long) const> delegate;
		delegate.Bind(constLambdaAddInt);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(constLambdaAddInt));
		BRICXX_CHECK(delegate(1, 2) == 3);
	}
	
	// 5.3 member
	{
		ConstClassAddInt constClassAddInt;
		
		Delegate<long (long, long) const> delegate;
		delegate.Bind<&ConstClassAddInt::Add>(constClassAddInt);
		BRICXX_CHECK(delegate.IsBinded());
		bool constClassAddIntIsBindedTo = delegate.IsBindedTo<&ConstClassAddInt::Add>(constClassAddInt);
		BRICXX_CHECK(constClassAddIntIsBindedTo);
		BRICXX_CHECK(delegate(1, 2) == 3);
	}
}

void TestNoexceptDelegate() {
	// 1 constructor, ::std::terminate()/BadDelegateCall
	{
		Delegate<void () noexcept> delegate;
		BRICXX_CHECK(!delegate.IsBinded());
		//delegate();	// OK, calls ::std::terminate()
	}
	
	
	//************************************************************
	// 2 bind/unbind
	
	// 2.1 function
	{
		Delegate<long (long, long) noexcept> delegate;
		delegate.Bind<&NoexceptFunctionAddLong>();
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo<&NoexceptFunctionAddLong>());
		BRICXX_CHECK(delegate(1L, 2L) == 3L);
		delegate.Unbind();
		BRICXX_CHECK(!delegate.IsBinded());
	}
	
	// 2.2 functor
	{
		NoexceptFunctorAddLong noexceptFunctorAddLong;
		
		Delegate<long (long, long) noexcept> delegate;
		delegate.Bind(noexceptFunctorAddLong);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(noexceptFunctorAddLong));
		BRICXX_CHECK(delegate(1L, 2L) == 3L);
		BRICXX_CHECK(noexceptFunctorAddLong.GetNumCalls() == 1);
		delegate.Unbind();
		BRICXX_CHECK(!delegate.IsBinded());
	}
	
	// 2.3 lambda
	{
		int numCalls = 0;
		auto noexceptLambdaAddLong = [&numCalls](long x, long y) mutable noexcept -> long {
			++numCalls;
			return x + y;
		};
		
		Delegate<long (long, long) noexcept> delegate;
		delegate.Bind(noexceptLambdaAddLong);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(noexceptLambdaAddLong));
		BRICXX_CHECK(delegate(1L, 2L) == 3L);
		BRICXX_CHECK(numCalls == 1);
		delegate.Unbind();
		BRICXX_CHECK(!delegate.IsBinded());
	}
	
	// 2.4 member
	{
		NoexceptClassAddLong noexceptClassAddLong;
		
		Delegate<long (long, long) noexcept> delegate;
		delegate.Bind<&NoexceptClassAddLong::Add>(noexceptClassAddLong);
		BRICXX_CHECK(delegate.IsBinded());
		bool noexceptClassAddLongIsBindedTo = delegate.IsBindedTo<&NoexceptClassAddLong::Add>(noexceptClassAddLong);
		BRICXX_CHECK(noexceptClassAddLongIsBindedTo);
		BRICXX_CHECK(delegate(1L, 2L) == 3L);
		BRICXX_CHECK(noexceptClassAddLong.GetNumCalls() == 1);
		delegate.Unbind();
		BRICXX_CHECK(!delegate.IsBinded());
	}
	
	
	//************************************************************
	// 3 copy constructor/assignment, equality
	
	// 3.1 function
	{
		Delegate<long (long, long) noexcept> delegate;
		delegate.Bind<&NoexceptFunctionAddLong>();
		
		Delegate<long (long, long) noexcept> delegate2 = delegate;
		BRICXX_CHECK(delegate2.IsBinded());
		BRICXX_CHECK(delegate2.IsBindedTo<&NoexceptFunctionAddLong>());
		BRICXX_CHECK(delegate2(1L, 2L) == 3L);
		
		Delegate<long (long, long) noexcept> delegate3;
		delegate3 = delegate;
		BRICXX_CHECK(delegate3.IsBinded());
		BRICXX_CHECK(delegate3.IsBindedTo<&NoexceptFunctionAddLong>());
		BRICXX_CHECK(delegate3(2L, 3L) == 5L);
		
		BRICXX_CHECK(delegate2 == delegate3);
		delegate3.Unbind();
		BRICXX_CHECK(delegate2 != delegate3);
	}
	
	// 3.2 functor
	{
		NoexceptFunctorAddLong noexceptFunctorAddLong;
		
		Delegate<long (long, long) noexcept> delegate;
		delegate.Bind(noexceptFunctorAddLong);
		
		Delegate<long (long, long) noexcept> delegate2 = delegate;
		BRICXX_CHECK(delegate2.IsBinded());
		BRICXX_CHECK(delegate2.IsBindedTo(noexceptFunctorAddLong));
		BRICXX_CHECK(delegate2(1L, 2L) == 3L);
		BRICXX_CHECK(noexceptFunctorAddLong.GetNumCalls() == 1);
		
		Delegate<long (long, long) noexcept> delegate3;
		delegate3 = delegate;
		BRICXX_CHECK(delegate3.IsBinded());
		BRICXX_CHECK(delegate3.IsBindedTo(noexceptFunctorAddLong));
		BRICXX_CHECK(delegate3(2L, 3L) == 5L);
		BRICXX_CHECK(noexceptFunctorAddLong.GetNumCalls() == 2);
		
		BRICXX_CHECK(delegate2 == delegate3);
		delegate3.Unbind();
		BRICXX_CHECK(delegate2 != delegate3);
	}
	
	// 3.3 lambda
	{
		int numCalls = 0;
		auto noexceptLambdaAddLong = [&numCalls](long x, long y) mutable noexcept -> long {
			++numCalls;
			return x + y;
		};
		
		Delegate<long (long, long) noexcept> delegate;
		delegate.Bind(noexceptLambdaAddLong);
		
		Delegate<long (long, long) noexcept> delegate2 = delegate;
		BRICXX_CHECK(delegate2.IsBinded());
		BRICXX_CHECK(delegate2.IsBindedTo(noexceptLambdaAddLong));
		BRICXX_CHECK(delegate2(1L, 2L) == 3L);
		BRICXX_CHECK(numCalls == 1);
		
		Delegate<long (long, long) noexcept> delegate3;
		delegate3 = delegate;
		BRICXX_CHECK(delegate3.IsBinded());
		BRICXX_CHECK(delegate3.IsBindedTo(noexceptLambdaAddLong));
		BRICXX_CHECK(delegate3(2L, 3L) == 5L);
		BRICXX_CHECK(numCalls == 2);
		
		BRICXX_CHECK(delegate2 == delegate3);
		delegate3.Unbind();
		BRICXX_CHECK(delegate2 != delegate3);
	}
	
	// 3.4 member
	{
		NoexceptClassAddLong noexceptClassAddLong;
		
		Delegate<long (long, long) noexcept> delegate;
		delegate.Bind<&NoexceptClassAddLong::Add>(noexceptClassAddLong);
		
		Delegate<long (long, long) noexcept> delegate2 = delegate;
		BRICXX_CHECK(delegate2.IsBinded());
		bool noexceptClassAddLongIsBindedTo = delegate2.IsBindedTo<&NoexceptClassAddLong::Add>(noexceptClassAddLong);
		BRICXX_CHECK(noexceptClassAddLongIsBindedTo);
		BRICXX_CHECK(delegate2(1L, 2L) == 3L);
		BRICXX_CHECK(noexceptClassAddLong.GetNumCalls() == 1);
		
		Delegate<long (long, long) noexcept> delegate3;
		delegate3 = delegate;
		BRICXX_CHECK(delegate3.IsBinded());
		noexceptClassAddLongIsBindedTo = delegate3.IsBindedTo<&NoexceptClassAddLong::Add>(noexceptClassAddLong);
		BRICXX_CHECK(noexceptClassAddLongIsBindedTo);
		BRICXX_CHECK(delegate3(2L, 3L) == 5L);
		BRICXX_CHECK(noexceptClassAddLong.GetNumCalls() == 2);
		
		BRICXX_CHECK(delegate2 == delegate3);
		delegate3.Unbind();
		BRICXX_CHECK(delegate2 != delegate3);
	}
	
	
	//************************************************************
	// 4 noexcept-correctness
	
	// 4.1 function
	{
		Delegate<long (long, long) noexcept> delegate;
		//delegate.Bind<&FunctionAddLong>();	// OK, does not compile
		(void)delegate;
	}
	
	// 4.2 functor
	{
		FunctorAddLong functorAddLong;
		
		Delegate<long (long, long) noexcept> delegate;
		//delegate.Bind(functorAddLong);	// OK, does not compile
		(void)functorAddLong;
		(void)delegate;
	}
	
	// 4.3 lambda
	{
		int numCalls = 0;
		auto lambdaAddLong = [&numCalls](long x, long y) mutable -> long {
			++numCalls;
			return x + y;
		};
		
		Delegate<long (long, long) noexcept> delegate;
		//delegate.Bind(lambdaAddLong);	// OK, does not compile
		(void)lambdaAddLong;
		(void)delegate;
	}
	
	// 4.4 member
	{
		ClassAddLong classAddLong;
		
		Delegate<long (long, long) noexcept> delegate;
		//delegate.Bind<&ClassAddLong::Add>(classAddLong);	// OK, does not compile
		(void)classAddLong;
		(void)delegate;
	}
	
	
	//************************************************************
	// 5 compatible signatures
	
	// 5.1 function
	{
		Delegate<long (long, long) noexcept> delegate;
		delegate.Bind<&NoexceptFunctionAddInt>();
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo<&NoexceptFunctionAddInt>());
		BRICXX_CHECK(delegate(1, 2) == 3);
	}
	
	// 5.2 functor
	{
		NoexceptFunctorAddInt noexceptFunctorAddInt;
		
		Delegate<long (long, long) noexcept> delegate;
		delegate.Bind(noexceptFunctorAddInt);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(noexceptFunctorAddInt));
		BRICXX_CHECK(delegate(1, 2) == 3);
		BRICXX_CHECK(noexceptFunctorAddInt.GetNumCalls() == 1);
	}
	
	// 5.3 lambda
	{
		int numCalls = 0;
		auto noexceptLambdaAddInt = [&numCalls](int x, int y) mutable noexcept -> int {
			++numCalls;
			return x + y;
		};
		
		Delegate<long (long, long) noexcept> delegate;
		delegate.Bind(noexceptLambdaAddInt);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(noexceptLambdaAddInt));
		BRICXX_CHECK(delegate(1, 2) == 3);
		BRICXX_CHECK(numCalls == 1);
	}
	
	// 5.4 member
	{
		NoexceptClassAddInt noexceptClassAddInt;
		
		Delegate<long (long, long) noexcept> delegate;
		delegate.Bind<&NoexceptClassAddInt::Add>(noexceptClassAddInt);
		BRICXX_CHECK(delegate.IsBinded());
		bool noexceptClassAddIntIsBindedTo = delegate.IsBindedTo<&NoexceptClassAddInt::Add>(noexceptClassAddInt);
		BRICXX_CHECK(noexceptClassAddIntIsBindedTo);
		BRICXX_CHECK(delegate(1, 2) == 3);
		BRICXX_CHECK(noexceptClassAddInt.GetNumCalls() == 1);
	}
}

void TestConstNoexceptDelegate() {
	// 1 constructor, ::std::terminate()/BadDelegateCall
	{
		Delegate<void () const noexcept> delegate;
		BRICXX_CHECK(!delegate.IsBinded());
		//delegate();	// OK, calls ::std::terminate()
	}
	
	
	//************************************************************
	// 2 bind/unbind
	
	// 2.1 functor
	{
		ConstNoexceptFunctorAddLong constNoexceptFunctorAddLong;
		
		Delegate<long (long, long) const noexcept> delegate;
		delegate.Bind(constNoexceptFunctorAddLong);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(constNoexceptFunctorAddLong));
		BRICXX_CHECK(delegate(1L, 2L) == 3L);
		delegate.Unbind();
		BRICXX_CHECK(!delegate.IsBinded());
	}
	
	// 2.2 lambda
	{
		auto constNoexceptLambdaAddLong = [](long x, long y) noexcept -> long {
			return x + y;
		};
		
		Delegate<long (long, long) const noexcept> delegate;
		delegate.Bind(constNoexceptLambdaAddLong);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(constNoexceptLambdaAddLong));
		BRICXX_CHECK(delegate(1L, 2L) == 3L);
		delegate.Unbind();
		BRICXX_CHECK(!delegate.IsBinded());
	}
	
	// 2.3 member
	{
		ConstNoexceptClassAddLong constNoexceptClassAddLong;
		
		Delegate<long (long, long) const noexcept> delegate;
		delegate.Bind<&ConstNoexceptClassAddLong::Add>(constNoexceptClassAddLong);
		BRICXX_CHECK(delegate.IsBinded());
		bool constNoexceptClassAddLongIsBindedTo = delegate.IsBindedTo<&ConstNoexceptClassAddLong::Add>(constNoexceptClassAddLong);
		BRICXX_CHECK(constNoexceptClassAddLongIsBindedTo);
		BRICXX_CHECK(delegate(1L, 2L) == 3L);
		delegate.Unbind();
		BRICXX_CHECK(!delegate.IsBinded());
	}
	
	
	//************************************************************
	// 3 copy constructor/assignment, equality
	
	// 3.1 functor
	{
		ConstNoexceptFunctorAddLong constNoexceptFunctorAddLong;
		
		Delegate<long (long, long) const noexcept> delegate;
		delegate.Bind(constNoexceptFunctorAddLong);
		
		Delegate<long (long, long) const noexcept> delegate2 = delegate;
		BRICXX_CHECK(delegate2.IsBinded());
		BRICXX_CHECK(delegate2.IsBindedTo(constNoexceptFunctorAddLong));
		BRICXX_CHECK(delegate2(1L, 2L) == 3L);
		
		Delegate<long (long, long) const noexcept> delegate3;
		delegate3 = delegate;
		BRICXX_CHECK(delegate3.IsBinded());
		BRICXX_CHECK(delegate3.IsBindedTo(constNoexceptFunctorAddLong));
		BRICXX_CHECK(delegate3(2L, 3L) == 5L);
		
		BRICXX_CHECK(delegate2 == delegate3);
		delegate3.Unbind();
		BRICXX_CHECK(delegate2 != delegate3);
	}
	
	// 3.2 lambda
	{
		auto constNoexceptLambdaAddLong = [](long x, long y) noexcept -> long {
			return x + y;
		};
		
		Delegate<long (long, long) const noexcept> delegate;
		delegate.Bind(constNoexceptLambdaAddLong);
		
		Delegate<long (long, long) const noexcept> delegate2 = delegate;
		BRICXX_CHECK(delegate2.IsBinded());
		BRICXX_CHECK(delegate2.IsBindedTo(constNoexceptLambdaAddLong));
		BRICXX_CHECK(delegate2(1L, 2L) == 3L);
		
		Delegate<long (long, long) const noexcept> delegate3;
		delegate3 = delegate;
		BRICXX_CHECK(delegate3.IsBinded());
		BRICXX_CHECK(delegate3.IsBindedTo(constNoexceptLambdaAddLong));
		BRICXX_CHECK(delegate3(2L, 3L) == 5L);
		
		BRICXX_CHECK(delegate2 == delegate3);
		delegate3.Unbind();
		BRICXX_CHECK(delegate2 != delegate3);
	}
	
	// 3.3 member
	{
		ConstNoexceptClassAddLong constNoexceptClassAddLong;
		
		Delegate<long (long, long) const noexcept> delegate;
		delegate.Bind<&ConstNoexceptClassAddLong::Add>(constNoexceptClassAddLong);
		
		Delegate<long (long, long) const noexcept> delegate2 = delegate;
		BRICXX_CHECK(delegate2.IsBinded());
		bool constNoexceptClassAddLongIsBindedTo = delegate2.IsBindedTo<&ConstNoexceptClassAddLong::Add>(constNoexceptClassAddLong);
		BRICXX_CHECK(constNoexceptClassAddLongIsBindedTo);
		BRICXX_CHECK(delegate2(1L, 2L) == 3L);
		
		Delegate<long (long, long) const noexcept> delegate3;
		delegate3 = delegate;
		BRICXX_CHECK(delegate3.IsBinded());
		constNoexceptClassAddLongIsBindedTo = delegate3.IsBindedTo<&ConstNoexceptClassAddLong::Add>(constNoexceptClassAddLong);
		BRICXX_CHECK(constNoexceptClassAddLongIsBindedTo);
		BRICXX_CHECK(delegate3(2L, 3L) == 5L);
		
		BRICXX_CHECK(delegate2 == delegate3);
		delegate3.Unbind();
		BRICXX_CHECK(delegate2 != delegate3);
	}
	
	
	//************************************************************
	// 4 const-noexcept-correctness
	
	// 4.1 functor
	{
		Delegate<long (long, long) const noexcept> delegate;
		
		FunctorAddLong functorAddLong;
		//delegate.Bind(functorAddLong);	// OK, does not compile
		(void)functorAddLong;
		
		ConstFunctorAddLong constFunctorAddLong;
		//delegate.Bind(constFunctorAddLong);	// OK, does not compile
		(void)constFunctorAddLong;
		
		NoexceptFunctorAddLong noexceptFunctorAddLong;
		//delegate.Bind(noexceptFunctorAddLong);	// OK, does not compile
		(void)noexceptFunctorAddLong;
		
		(void)delegate;
	}
	
	// 4.2 lambda
	{
		Delegate<long (long, long) const noexcept> delegate;
		
		int numCalls = 0;
		auto lambdaAddLong = [&numCalls](long x, long y) mutable -> long {
			++numCalls;
			return x + y;
		};
		//delegate.Bind(lambdaAddLong);	// OK, does not compile
		(void)lambdaAddLong;
		
		auto constLambdaAddLong = [](long x, long y) -> long {
			return x + y;
		};
		//delegate.Bind(constLambdaAddLong);	// OK, does not compile
		(void)constLambdaAddLong;
		
		auto noexceptLambdaAddLong = [&numCalls](long x, long y) mutable noexcept -> long {
			++numCalls;
			return x + y;
		};
		//delegate.Bind(noexceptLambdaAddLong);	// OK, does not compile
		(void)noexceptLambdaAddLong;
		
		(void)delegate;
	}
	
	// 4.3 member
	{
		Delegate<long (long, long) const noexcept> delegate;
		
		ClassAddLong classAddLong;
		//delegate.Bind<&ClassAddLong::Add>(classAddLong);	// OK, does not compile
		(void)classAddLong;
		
		ConstClassAddLong constClassAddLong;
		//delegate.Bind<&ConstClassAddLong::Add>(constClassAddLong);	// OK, does not compile
		(void)constClassAddLong;
		
		NoexceptClassAddLong noexceptClassAddLong;
		//delegate.Bind<&NoexceptClassAddLong::Add>(noexceptClassAddLong);	// OK, does not compile
		(void)noexceptClassAddLong;
		
		(void)delegate;
	}
	
	
	//************************************************************
	// 5 compatible signatures
	
	// 5.1 functor
	{
		ConstNoexceptFunctorAddInt constNoexceptFunctorAddInt;
		
		Delegate<long (long, long) const> delegate;
		delegate.Bind(constNoexceptFunctorAddInt);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(constNoexceptFunctorAddInt));
		BRICXX_CHECK(delegate(1, 2) == 3);
	}
	
	// 5.2 lambda
	{
		auto constNoexceptLambdaAddInt = [](int x, int y) noexcept -> int {
			return x + y;
		};
		
		Delegate<long (long, long) const> delegate;
		delegate.Bind(constNoexceptLambdaAddInt);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(constNoexceptLambdaAddInt));
		BRICXX_CHECK(delegate(1, 2) == 3);
	}
	
	// 5.3 member
	{
		ConstNoexceptClassAddInt constNoexceptClassAddInt;
		
		Delegate<long (long, long) const> delegate;
		delegate.Bind<&ConstNoexceptClassAddInt::Add>(constNoexceptClassAddInt);
		BRICXX_CHECK(delegate.IsBinded());
		bool constNoexceptClassAddIntIsBindedTo = delegate.IsBindedTo<&ConstNoexceptClassAddInt::Add>(constNoexceptClassAddInt);
		BRICXX_CHECK(constNoexceptClassAddIntIsBindedTo);
		BRICXX_CHECK(delegate(1, 2) == 3);
	}
}


inline long FunctionAddLong(long x, long y) {
	return x + y;
}

inline long FunctorAddLong::operator()(long x, long y) {
	++numCalls_;
	return x + y;
}

inline int FunctorAddLong::GetNumCalls() const {
	return numCalls_;
}

inline long ClassAddLong::Add(long x, long y) {
	++numCalls_;
	return x + y;
}

inline int ClassAddLong::GetNumCalls() const {
	return numCalls_;
}


inline long ConstFunctorAddLong::operator()(long x, long y) const {
	return x + y;
}

inline long ConstClassAddLong::Add(long x, long y) const {
	return x + y;
}


inline long NoexceptFunctionAddLong(long x, long y) noexcept {
	return x + y;
}

inline long NoexceptFunctorAddLong::operator()(long x, long y) noexcept {
	++numCalls_;
	return x + y;
}

inline int NoexceptFunctorAddLong::GetNumCalls() const {
	return numCalls_;
}

inline long NoexceptClassAddLong::Add(long x, long y) noexcept {
	++numCalls_;
	return x + y;
}

inline int NoexceptClassAddLong::GetNumCalls() const {
	return numCalls_;
}


inline long ConstNoexceptFunctorAddLong::operator()(long x, long y) const noexcept {
	return x + y;
}

inline long ConstNoexceptClassAddLong::Add(long x, long y) const noexcept {
	return x + y;
}


inline int FunctionAddInt(int x, int y) {
	return x + y;
}

inline int FunctorAddInt::operator()(int x, int y) {
	++numCalls_;
	return x + y;
}

inline int FunctorAddInt::GetNumCalls() const {
	return numCalls_;
}

inline int ClassAddInt::Add(int x, int y) {
	++numCalls_;
	return x + y;
}

inline int ClassAddInt::GetNumCalls() const {
	return numCalls_;
}


inline int ConstFunctorAddInt::operator()(int x, int y) const {
	return x + y;
}

inline int ConstClassAddInt::Add(int x, int y) const {
	return x + y;
}


inline int NoexceptFunctionAddInt(int x, int y) noexcept {
	return x + y;
}

inline int NoexceptFunctorAddInt::operator()(int x, int y) noexcept {
	++numCalls_;
	return x + y;
}

inline int NoexceptFunctorAddInt::GetNumCalls() const {
	return numCalls_;
}

inline int NoexceptClassAddInt::Add(int x, int y) noexcept {
	++numCalls_;
	return x + y;
}

inline int NoexceptClassAddInt::GetNumCalls() const {
	return numCalls_;
}


inline int ConstNoexceptFunctorAddInt::operator()(int x, int y) const noexcept {
	return x + y;
}

inline int ConstNoexceptClassAddInt::Add(int x, int y) const noexcept {
	return x + y;
}




int main() try {
	TestHeaderDoc();
	TestDelegate();
	TestConstDelegate();
	TestNoexceptDelegate();
	TestConstNoexceptDelegate();
	
	cout << "test completed" << endl;
	return EXIT_SUCCESS;
}
catch (const exception& exc) {
	cout << "test failed: " << exc.what() << endl;
	return EXIT_FAILURE;
}
