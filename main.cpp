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


int FunctionAddInt(int x, int y);

class FunctorAddInt {
public:
	int operator()(int x, int y);
	
	unsigned int GetNumCalls() const;
private:
	unsigned int numCalls_ = 0U;
};

class ClassAddInt {
public:
	int Add(int x, int y);
	
	unsigned int GetNumCalls() const;
private:
	unsigned int numCalls_ = 0U;
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
	
	unsigned int GetNumCalls() const;
private:
	unsigned int numCalls_ = 0U;
};

class NoexceptClassAddInt {
public:
	int Add(int x, int y) noexcept;
	
	unsigned int GetNumCalls() const;
private:
	unsigned int numCalls_ = 0U;
};


class ConstNoexceptFunctorAddInt {
public:
	int operator()(int x, int y) const noexcept;
};

class ConstNoexceptClassAddInt {
public:
	int Add(int x, int y) const noexcept;
};


long FunctionAddLong(long x, long y);

class FunctorAddLong {
public:
	long operator()(long x, long y);
	
	unsigned int GetNumCalls() const;
private:
	unsigned int numCalls_ = 0U;
};

class ClassAddLong {
public:
	long Add(long x, long y);
	
	unsigned int GetNumCalls() const;
private:
	unsigned int numCalls_ = 0U;
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
	
	unsigned int GetNumCalls() const;
private:
	unsigned int numCalls_ = 0U;
};

class NoexceptClassAddLong {
public:
	long Add(long x, long y) noexcept;
	
	unsigned int GetNumCalls() const;
private:
	unsigned int numCalls_ = 0U;
};


class ConstNoexceptFunctorAddLong {
public:
	long operator()(long x, long y) const noexcept;
};

class ConstNoexceptClassAddLong {
public:
	long Add(long x, long y) const noexcept;
};


void TestHeaderDoc() {
	// (static member) function
	Delegate<int (int)> functionDelegate;
	functionDelegate.Bind<&::islower>();
	if (!functionDelegate.IsBindedTo<&::islower>()) throw runtime_error("function test failed");
	if (!functionDelegate('a')) throw runtime_error("function test failed");
	
	// lambda/functor
	unsigned int numCalls = 0U;
	auto lambda = [&numCalls](int x, int y) mutable -> bool {
		++numCalls;
		return x > y;
	};
	Delegate<bool (int, int)> lambdaDelegate;
	lambdaDelegate.Bind(lambda);
	if (!lambdaDelegate.IsBindedTo(lambda)) throw runtime_error("lambda test failed");
	if (!lambdaDelegate(2, 1)) throw runtime_error("lambda test failed");
	if (numCalls != 1U) throw runtime_error("lambda test failed");
	
	// non-static member function
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
	Delegate<long (const char*)> delegate;
	delegate.Bind<&::std::atoi>();	// int atoi(const char* str);
	if (!delegate.IsBindedTo<&::std::atoi>()) throw runtime_error("compatible signatures test failed");
	if (delegate("10") != 10L) throw runtime_error("compatible signatures test failed");
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
	
	// 2.1 (static member) function
	{
		Delegate<int (int, int)> delegate;
		delegate.Bind<&FunctionAddInt>();
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo<&FunctionAddInt>());
		BRICXX_CHECK(delegate(1, 2) == 3);
		delegate.Unbind();
		BRICXX_CHECK(!delegate.IsBinded());
		
		Delegate delegate1 = Delegate<int (int, int)>::CreateAndBind<&FunctionAddInt>();
		BRICXX_CHECK(delegate1.IsBinded());
		BRICXX_CHECK(delegate1.IsBindedTo<&FunctionAddInt>());
		BRICXX_CHECK(delegate1(1, 2) == 3);
	}
	
	// 2.2 functor
	{
		FunctorAddInt functorAddInt;
		
		Delegate<int (int, int)> delegate;
		delegate.Bind(functorAddInt);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(functorAddInt));
		BRICXX_CHECK(delegate(1, 2) == 3);
		BRICXX_CHECK(functorAddInt.GetNumCalls() == 1U);
		delegate.Unbind();
		BRICXX_CHECK(!delegate.IsBinded());
		
		Delegate delegate1 = Delegate<int (int, int)>::CreateAndBind(functorAddInt);
		BRICXX_CHECK(delegate1.IsBinded());
		BRICXX_CHECK(delegate1.IsBindedTo(functorAddInt));
		BRICXX_CHECK(delegate1(1, 2) == 3);
		BRICXX_CHECK(functorAddInt.GetNumCalls() == 2U);
	}
	
	// 2.3 lambda
	{
		unsigned int numCalls = 0U;
		auto lambdaAddInt = [&numCalls](int x, int y) mutable -> int {
			++numCalls;
			return x + y;
		};
		
		Delegate<int (int, int)> delegate;
		delegate.Bind(lambdaAddInt);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(lambdaAddInt));
		BRICXX_CHECK(delegate(1, 2) == 3);
		BRICXX_CHECK(numCalls == 1U);
		delegate.Unbind();
		BRICXX_CHECK(!delegate.IsBinded());
		
		Delegate delegate1 = Delegate<int (int, int)>::CreateAndBind(lambdaAddInt);
		BRICXX_CHECK(delegate1.IsBinded());
		BRICXX_CHECK(delegate1.IsBindedTo(lambdaAddInt));
		BRICXX_CHECK(delegate1(1, 2) == 3);
		BRICXX_CHECK(numCalls == 2U);
	}
	
	// 2.4 non-static member function
	{
		ClassAddInt classAddInt;
		
		Delegate<int (int, int)> delegate;
		delegate.Bind<&ClassAddInt::Add>(classAddInt);
		BRICXX_CHECK(delegate.IsBinded());
		bool classAddIntIsBindedTo = delegate.IsBindedTo<&ClassAddInt::Add>(classAddInt);
		BRICXX_CHECK(classAddIntIsBindedTo);
		BRICXX_CHECK(delegate(1, 2) == 3);
		BRICXX_CHECK(classAddInt.GetNumCalls() == 1U);
		delegate.Unbind();
		BRICXX_CHECK(!delegate.IsBinded());
		
		Delegate delegate1 = Delegate<int (int, int)>::CreateAndBind<&ClassAddInt::Add>(classAddInt);
		BRICXX_CHECK(delegate1.IsBinded());
		classAddIntIsBindedTo = delegate1.IsBindedTo<&ClassAddInt::Add>(classAddInt);
		BRICXX_CHECK(classAddIntIsBindedTo);
		BRICXX_CHECK(delegate1(1, 2) == 3);
		BRICXX_CHECK(classAddInt.GetNumCalls() == 2U);
	}
	
	
	//************************************************************
	// 3 copy constructor/assignment, equality
	
	// 3.1 (static member) function
	{
		Delegate<int (int, int)> delegate;
		delegate.Bind<&FunctionAddInt>();
		
		Delegate<int (int, int)> delegate2 = delegate;
		BRICXX_CHECK(delegate2.IsBinded());
		BRICXX_CHECK(delegate2.IsBindedTo<&FunctionAddInt>());
		BRICXX_CHECK(delegate2(1, 2) == 3);
		
		Delegate<int (int, int)> delegate3;
		delegate3 = delegate;
		BRICXX_CHECK(delegate3.IsBinded());
		BRICXX_CHECK(delegate3.IsBindedTo<&FunctionAddInt>());
		BRICXX_CHECK(delegate3(2, 3) == 5);
		
		BRICXX_CHECK(delegate2 == delegate3);
		delegate3.Unbind();
		BRICXX_CHECK(delegate2 != delegate3);
	}
	
	// 3.2 functor
	{
		FunctorAddInt functorAddInt;
		
		Delegate<int (int, int)> delegate;
		delegate.Bind(functorAddInt);
		
		Delegate<int (int, int)> delegate2 = delegate;
		BRICXX_CHECK(delegate2.IsBinded());
		BRICXX_CHECK(delegate2.IsBindedTo(functorAddInt));
		BRICXX_CHECK(delegate2(1, 2) == 3);
		BRICXX_CHECK(functorAddInt.GetNumCalls() == 1U);
		
		Delegate<int (int, int)> delegate3;
		delegate3 = delegate;
		BRICXX_CHECK(delegate3.IsBinded());
		BRICXX_CHECK(delegate3.IsBindedTo(functorAddInt));
		BRICXX_CHECK(delegate3(2, 3) == 5);
		BRICXX_CHECK(functorAddInt.GetNumCalls() == 2U);
		
		BRICXX_CHECK(delegate2 == delegate3);
		delegate3.Unbind();
		BRICXX_CHECK(delegate2 != delegate3);
	}
	
	// 3.3 lambda
	{
		unsigned int numCalls = 0U;
		auto lambdaAddInt = [&numCalls](int x, int y) mutable -> int {
			++numCalls;
			return x + y;
		};
		
		Delegate<int (int, int)> delegate;
		delegate.Bind(lambdaAddInt);
		
		Delegate<int (int, int)> delegate2 = delegate;
		BRICXX_CHECK(delegate2.IsBinded());
		BRICXX_CHECK(delegate2.IsBindedTo(lambdaAddInt));
		BRICXX_CHECK(delegate2(1, 2) == 3);
		BRICXX_CHECK(numCalls == 1U);
		
		Delegate<int (int, int)> delegate3;
		delegate3 = delegate;
		BRICXX_CHECK(delegate3.IsBinded());
		BRICXX_CHECK(delegate3.IsBindedTo(lambdaAddInt));
		BRICXX_CHECK(delegate3(2, 3) == 5);
		BRICXX_CHECK(numCalls == 2U);
		
		BRICXX_CHECK(delegate2 == delegate3);
		delegate3.Unbind();
		BRICXX_CHECK(delegate2 != delegate3);
	}
	
	// 3.4 non-static member function
	{
		ClassAddInt classAddInt;
		
		Delegate<int (int, int)> delegate;
		delegate.Bind<&ClassAddInt::Add>(classAddInt);
		
		Delegate<int (int, int)> delegate2 = delegate;
		BRICXX_CHECK(delegate2.IsBinded());
		bool classAddIntIsBindedTo = delegate2.IsBindedTo<&ClassAddInt::Add>(classAddInt);
		BRICXX_CHECK(classAddIntIsBindedTo);
		BRICXX_CHECK(delegate2(1, 2) == 3);
		BRICXX_CHECK(classAddInt.GetNumCalls() == 1U);
		
		Delegate<int (int, int)> delegate3;
		delegate3 = delegate;
		BRICXX_CHECK(delegate3.IsBinded());
		classAddIntIsBindedTo = delegate3.IsBindedTo<&ClassAddInt::Add>(classAddInt);
		BRICXX_CHECK(classAddIntIsBindedTo);
		BRICXX_CHECK(delegate3(2, 3) == 5);
		BRICXX_CHECK(classAddInt.GetNumCalls() == 2U);
		
		BRICXX_CHECK(delegate2 == delegate3);
		delegate3.Unbind();
		BRICXX_CHECK(delegate2 != delegate3);
	}
	
	
	//************************************************************
	// 4 compatible signatures
	
	// 4.1 (static member) function
	{
		Delegate<int (int, int)> delegate;
		delegate.Bind<&FunctionAddLong>();
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo<&FunctionAddLong>());
		BRICXX_CHECK(delegate(1, 2) == 3);
	}
	
	// 4.2 functor
	{
		FunctorAddLong functorAddLong;
		
		Delegate<int (int, int)> delegate;
		delegate.Bind(functorAddLong);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(functorAddLong));
		BRICXX_CHECK(delegate(1, 2) == 3);
		BRICXX_CHECK(functorAddLong.GetNumCalls() == 1U);
	}
	
	// 4.3 lambda
	{
		unsigned int numCalls = 0U;
		auto lambdaAddLong = [&numCalls](long x, long y) mutable -> long {
			++numCalls;
			return x + y;
		};
		
		Delegate<int (int, int)> delegate;
		delegate.Bind(lambdaAddLong);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(lambdaAddLong));
		BRICXX_CHECK(delegate(1, 2) == 3);
		BRICXX_CHECK(numCalls == 1U);
	}
	
	// 4.4 non-static member function
	{
		ClassAddLong classAddLong;
		
		Delegate<int (int, int)> delegate;
		delegate.Bind<&ClassAddLong::Add>(classAddLong);
		BRICXX_CHECK(delegate.IsBinded());
		bool classAddLongIsBindedTo = delegate.IsBindedTo<&ClassAddLong::Add>(classAddLong);
		BRICXX_CHECK(classAddLongIsBindedTo);
		BRICXX_CHECK(delegate(1, 2) == 3);
		BRICXX_CHECK(classAddLong.GetNumCalls() == 1U);
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
	
	// 2.1 (static member) function
	{
		Delegate<int (int, int) const> delegate;
		delegate.Bind<&FunctionAddInt>();
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo<&FunctionAddInt>());
		BRICXX_CHECK(delegate(1, 2) == 3);
		delegate.Unbind();
		BRICXX_CHECK(!delegate.IsBinded());
		
		Delegate delegate1 = Delegate<int (int, int) const>::CreateAndBind<&FunctionAddInt>();
		BRICXX_CHECK(delegate1.IsBinded());
		BRICXX_CHECK(delegate1.IsBindedTo<&FunctionAddInt>());
		BRICXX_CHECK(delegate1(1, 2) == 3);
	}
	
	// 2.2 functor
	{
		ConstFunctorAddInt constFunctorAddInt;
		
		Delegate<int (int, int) const> delegate;
		delegate.Bind(constFunctorAddInt);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(constFunctorAddInt));
		BRICXX_CHECK(delegate(1, 2) == 3);
		delegate.Unbind();
		BRICXX_CHECK(!delegate.IsBinded());
		
		Delegate delegate1 = Delegate<int (int, int) const>::CreateAndBind(constFunctorAddInt);
		BRICXX_CHECK(delegate1.IsBinded());
		BRICXX_CHECK(delegate1.IsBindedTo(constFunctorAddInt));
		BRICXX_CHECK(delegate1(1, 2) == 3);
	}
	
	// 2.3 lambda
	{
		auto constLambdaAddInt = [](int x, int y) -> int {
			return x + y;
		};
		
		Delegate<int (int, int) const> delegate;
		delegate.Bind(constLambdaAddInt);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(constLambdaAddInt));
		BRICXX_CHECK(delegate(1, 2) == 3);
		delegate.Unbind();
		BRICXX_CHECK(!delegate.IsBinded());
		
		Delegate delegate1 = Delegate<int (int, int) const>::CreateAndBind(constLambdaAddInt);
		BRICXX_CHECK(delegate1.IsBinded());
		BRICXX_CHECK(delegate1.IsBindedTo(constLambdaAddInt));
		BRICXX_CHECK(delegate1(1, 2) == 3);
	}
	
	// 2.4 non-static member function
	{
		ConstClassAddInt constClassAddInt;
		
		Delegate<int (int, int) const> delegate;
		delegate.Bind<&ConstClassAddInt::Add>(constClassAddInt);
		BRICXX_CHECK(delegate.IsBinded());
		bool constClassAddIntIsBindedTo = delegate.IsBindedTo<&ConstClassAddInt::Add>(constClassAddInt);
		BRICXX_CHECK(constClassAddIntIsBindedTo);
		BRICXX_CHECK(delegate(1, 2) == 3);
		delegate.Unbind();
		BRICXX_CHECK(!delegate.IsBinded());
		
		Delegate delegate1 = Delegate<int (int, int) const>::CreateAndBind<&ConstClassAddInt::Add>(constClassAddInt);
		BRICXX_CHECK(delegate1.IsBinded());
		constClassAddIntIsBindedTo = delegate1.IsBindedTo<&ConstClassAddInt::Add>(constClassAddInt);
		BRICXX_CHECK(constClassAddIntIsBindedTo);
		BRICXX_CHECK(delegate1(1, 2) == 3);
	}
	
	
	//************************************************************
	// 3 copy constructor/assignment, equality
	
	// 3.1 (static member) function
	{
		Delegate<int (int, int) const> delegate;
		delegate.Bind<&FunctionAddInt>();
		
		Delegate<int (int, int) const> delegate2 = delegate;
		BRICXX_CHECK(delegate2.IsBinded());
		BRICXX_CHECK(delegate2.IsBindedTo<&FunctionAddInt>());
		BRICXX_CHECK(delegate2(1, 2) == 3);
		
		Delegate<int (int, int) const> delegate3;
		delegate3 = delegate;
		BRICXX_CHECK(delegate3.IsBinded());
		BRICXX_CHECK(delegate3.IsBindedTo<&FunctionAddInt>());
		BRICXX_CHECK(delegate3(2, 3) == 5);
		
		BRICXX_CHECK(delegate2 == delegate3);
		delegate3.Unbind();
		BRICXX_CHECK(delegate2 != delegate3);
	}
	
	// 3.2 functor
	{
		ConstFunctorAddInt constFunctorAddInt;
		
		Delegate<int (int, int) const> delegate;
		delegate.Bind(constFunctorAddInt);
		
		Delegate<int (int, int) const> delegate2 = delegate;
		BRICXX_CHECK(delegate2.IsBinded());
		BRICXX_CHECK(delegate2.IsBindedTo(constFunctorAddInt));
		BRICXX_CHECK(delegate2(1, 2) == 3);
		
		Delegate<int (int, int) const> delegate3;
		delegate3 = delegate;
		BRICXX_CHECK(delegate3.IsBinded());
		BRICXX_CHECK(delegate3.IsBindedTo(constFunctorAddInt));
		BRICXX_CHECK(delegate3(2, 3) == 5);
		
		BRICXX_CHECK(delegate2 == delegate3);
		delegate3.Unbind();
		BRICXX_CHECK(delegate2 != delegate3);
	}
	
	// 3.3 lambda
	{
		auto constLambdaAddInt = [](int x, int y) -> int {
			return x + y;
		};
		
		Delegate<int (int, int) const> delegate;
		delegate.Bind(constLambdaAddInt);
		
		Delegate<int (int, int) const> delegate2 = delegate;
		BRICXX_CHECK(delegate2.IsBinded());
		BRICXX_CHECK(delegate2.IsBindedTo(constLambdaAddInt));
		BRICXX_CHECK(delegate2(1, 2) == 3);
		
		Delegate<int (int, int) const> delegate3;
		delegate3 = delegate;
		BRICXX_CHECK(delegate3.IsBinded());
		BRICXX_CHECK(delegate3.IsBindedTo(constLambdaAddInt));
		BRICXX_CHECK(delegate3(2, 3) == 5);
		
		BRICXX_CHECK(delegate2 == delegate3);
		delegate3.Unbind();
		BRICXX_CHECK(delegate2 != delegate3);
	}
	
	// 3.4 non-static member function
	{
		ConstClassAddInt constClassAddInt;
		
		Delegate<int (int, int) const> delegate;
		delegate.Bind<&ConstClassAddInt::Add>(constClassAddInt);
		
		Delegate<int (int, int) const> delegate2 = delegate;
		BRICXX_CHECK(delegate2.IsBinded());
		bool constClassAddIntIsBindedTo = delegate2.IsBindedTo<&ConstClassAddInt::Add>(constClassAddInt);
		BRICXX_CHECK(constClassAddIntIsBindedTo);
		BRICXX_CHECK(delegate2(1, 2) == 3);
		
		Delegate<int (int, int) const> delegate3;
		delegate3 = delegate;
		BRICXX_CHECK(delegate3.IsBinded());
		constClassAddIntIsBindedTo = delegate3.IsBindedTo<&ConstClassAddInt::Add>(constClassAddInt);
		BRICXX_CHECK(constClassAddIntIsBindedTo);
		BRICXX_CHECK(delegate3(2, 3) == 5);
		
		BRICXX_CHECK(delegate2 == delegate3);
		delegate3.Unbind();
		BRICXX_CHECK(delegate2 != delegate3);
	}
	
	
	//************************************************************
	// 4 const-correctness
	
	// 4.1 functor
	{
		FunctorAddInt functorAddInt;
		
		Delegate<int (int, int) const> delegate;
		//delegate.Bind(functorAddInt);	// OK, does not compile
		(void)functorAddInt;
		(void)delegate;
	}
	
	// 4.2 lambda
	{
		unsigned int numCalls = 0U;
		auto lambdaAddInt = [&numCalls](int x, int y) mutable -> int {
			++numCalls;
			return x + y;
		};
		
		Delegate<int (int, int) const> delegate;
		//delegate.Bind(lambdaAddInt);	// OK, does not compile
		(void)lambdaAddInt;
		(void)delegate;
	}
	
	// 4.3 non-static member function
	{
		ClassAddInt classAddInt;
		
		Delegate<int (int, int) const> delegate;
		//delegate.Bind<&ClassAddInt::Add>(classAddInt);	// OK, does not compile
		(void)classAddInt;
		(void)delegate;
	}
	
	
	//************************************************************
	// 5 compatible signatures
	
	// 5.1 (static member) function
	{
		Delegate<int (int, int) const> delegate;
		delegate.Bind<&FunctionAddLong>();
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo<&FunctionAddLong>());
		BRICXX_CHECK(delegate(1, 2) == 3);
	}
	
	// 5.2 functor
	{
		ConstFunctorAddLong constFunctorAddLong;
		
		Delegate<int (int, int) const> delegate;
		delegate.Bind(constFunctorAddLong);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(constFunctorAddLong));
		BRICXX_CHECK(delegate(1, 2) == 3);
	}
	
	// 5.3 lambda
	{
		auto constLambdaAddLong = [](long x, long y) -> long {
			return x + y;
		};
		
		Delegate<int (int, int) const> delegate;
		delegate.Bind(constLambdaAddLong);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(constLambdaAddLong));
		BRICXX_CHECK(delegate(1, 2) == 3);
	}
	
	// 5.4 non-static member function
	{
		ConstClassAddLong constClassAddLong;
		
		Delegate<int (int, int) const> delegate;
		delegate.Bind<&ConstClassAddLong::Add>(constClassAddLong);
		BRICXX_CHECK(delegate.IsBinded());
		bool constClassAddLongIsBindedTo = delegate.IsBindedTo<&ConstClassAddLong::Add>(constClassAddLong);
		BRICXX_CHECK(constClassAddLongIsBindedTo);
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
	
	// 2.1 (static member) function
	{
		Delegate<int (int, int) noexcept> delegate;
		delegate.Bind<&NoexceptFunctionAddInt>();
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo<&NoexceptFunctionAddInt>());
		BRICXX_CHECK(delegate(1, 2) == 3);
		delegate.Unbind();
		BRICXX_CHECK(!delegate.IsBinded());
		
		Delegate delegate1 = Delegate<int (int, int) noexcept>::CreateAndBind<&NoexceptFunctionAddInt>();
		BRICXX_CHECK(delegate1.IsBinded());
		BRICXX_CHECK(delegate1.IsBindedTo<&NoexceptFunctionAddInt>());
		BRICXX_CHECK(delegate1(1, 2) == 3);
	}
	
	// 2.2 functor
	{
		NoexceptFunctorAddInt noexceptFunctorAddInt;
		
		Delegate<int (int, int) noexcept> delegate;
		delegate.Bind(noexceptFunctorAddInt);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(noexceptFunctorAddInt));
		BRICXX_CHECK(delegate(1, 2) == 3);
		BRICXX_CHECK(noexceptFunctorAddInt.GetNumCalls() == 1U);
		delegate.Unbind();
		BRICXX_CHECK(!delegate.IsBinded());
		
		Delegate delegate1 = Delegate<int (int, int) noexcept>::CreateAndBind(noexceptFunctorAddInt);
		BRICXX_CHECK(delegate1.IsBinded());
		BRICXX_CHECK(delegate1.IsBindedTo(noexceptFunctorAddInt));
		BRICXX_CHECK(delegate1(1, 2) == 3);
		BRICXX_CHECK(noexceptFunctorAddInt.GetNumCalls() == 2U);
	}
	
	// 2.3 lambda
	{
		unsigned int numCalls = 0U;
		auto noexceptLambdaAddInt = [&numCalls](int x, int y) mutable noexcept -> int {
			++numCalls;
			return x + y;
		};
		
		Delegate<int (int, int) noexcept> delegate;
		delegate.Bind(noexceptLambdaAddInt);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(noexceptLambdaAddInt));
		BRICXX_CHECK(delegate(1, 2) == 3);
		BRICXX_CHECK(numCalls == 1U);
		delegate.Unbind();
		BRICXX_CHECK(!delegate.IsBinded());
		
		Delegate delegate1 = Delegate<int (int, int) noexcept>::CreateAndBind(noexceptLambdaAddInt);
		BRICXX_CHECK(delegate1.IsBinded());
		BRICXX_CHECK(delegate1.IsBindedTo(noexceptLambdaAddInt));
		BRICXX_CHECK(delegate1(1, 2) == 3);
		BRICXX_CHECK(numCalls == 2U);
	}
	
	// 2.4 non-static member function
	{
		NoexceptClassAddInt noexceptClassAddInt;
		
		Delegate<int (int, int) noexcept> delegate;
		delegate.Bind<&NoexceptClassAddInt::Add>(noexceptClassAddInt);
		BRICXX_CHECK(delegate.IsBinded());
		bool noexceptClassAddIntIsBindedTo = delegate.IsBindedTo<&NoexceptClassAddInt::Add>(noexceptClassAddInt);
		BRICXX_CHECK(noexceptClassAddIntIsBindedTo);
		BRICXX_CHECK(delegate(1, 2) == 3);
		BRICXX_CHECK(noexceptClassAddInt.GetNumCalls() == 1U);
		delegate.Unbind();
		BRICXX_CHECK(!delegate.IsBinded());
		
		Delegate delegate1 = Delegate<int (int, int) noexcept>::CreateAndBind<&NoexceptClassAddInt::Add>(
			noexceptClassAddInt);
		BRICXX_CHECK(delegate1.IsBinded());
		noexceptClassAddIntIsBindedTo = delegate1.IsBindedTo<&NoexceptClassAddInt::Add>(noexceptClassAddInt);
		BRICXX_CHECK(noexceptClassAddIntIsBindedTo);
		BRICXX_CHECK(delegate1(1, 2) == 3);
		BRICXX_CHECK(noexceptClassAddInt.GetNumCalls() == 2U);
	}
	
	
	//************************************************************
	// 3 copy constructor/assignment, equality
	
	// 3.1 (static member) function
	{
		Delegate<int (int, int) noexcept> delegate;
		delegate.Bind<&NoexceptFunctionAddInt>();
		
		Delegate<int (int, int) noexcept> delegate2 = delegate;
		BRICXX_CHECK(delegate2.IsBinded());
		BRICXX_CHECK(delegate2.IsBindedTo<&NoexceptFunctionAddInt>());
		BRICXX_CHECK(delegate2(1, 2) == 3);
		
		Delegate<int (int, int) noexcept> delegate3;
		delegate3 = delegate;
		BRICXX_CHECK(delegate3.IsBinded());
		BRICXX_CHECK(delegate3.IsBindedTo<&NoexceptFunctionAddInt>());
		BRICXX_CHECK(delegate3(2, 3) == 5);
		
		BRICXX_CHECK(delegate2 == delegate3);
		delegate3.Unbind();
		BRICXX_CHECK(delegate2 != delegate3);
	}
	
	// 3.2 functor
	{
		NoexceptFunctorAddInt noexceptFunctorAddInt;
		
		Delegate<int (int, int) noexcept> delegate;
		delegate.Bind(noexceptFunctorAddInt);
		
		Delegate<int (int, int) noexcept> delegate2 = delegate;
		BRICXX_CHECK(delegate2.IsBinded());
		BRICXX_CHECK(delegate2.IsBindedTo(noexceptFunctorAddInt));
		BRICXX_CHECK(delegate2(1, 2) == 3);
		BRICXX_CHECK(noexceptFunctorAddInt.GetNumCalls() == 1U);
		
		Delegate<int (int, int) noexcept> delegate3;
		delegate3 = delegate;
		BRICXX_CHECK(delegate3.IsBinded());
		BRICXX_CHECK(delegate3.IsBindedTo(noexceptFunctorAddInt));
		BRICXX_CHECK(delegate3(2, 3) == 5);
		BRICXX_CHECK(noexceptFunctorAddInt.GetNumCalls() == 2U);
		
		BRICXX_CHECK(delegate2 == delegate3);
		delegate3.Unbind();
		BRICXX_CHECK(delegate2 != delegate3);
	}
	
	// 3.3 lambda
	{
		unsigned int numCalls = 0U;
		auto noexceptLambdaAddInt = [&numCalls](int x, int y) mutable noexcept -> int {
			++numCalls;
			return x + y;
		};
		
		Delegate<int (int, int) noexcept> delegate;
		delegate.Bind(noexceptLambdaAddInt);
		
		Delegate<int (int, int) noexcept> delegate2 = delegate;
		BRICXX_CHECK(delegate2.IsBinded());
		BRICXX_CHECK(delegate2.IsBindedTo(noexceptLambdaAddInt));
		BRICXX_CHECK(delegate2(1, 2) == 3);
		BRICXX_CHECK(numCalls == 1U);
		
		Delegate<int (int, int) noexcept> delegate3;
		delegate3 = delegate;
		BRICXX_CHECK(delegate3.IsBinded());
		BRICXX_CHECK(delegate3.IsBindedTo(noexceptLambdaAddInt));
		BRICXX_CHECK(delegate3(2, 3) == 5);
		BRICXX_CHECK(numCalls == 2U);
		
		BRICXX_CHECK(delegate2 == delegate3);
		delegate3.Unbind();
		BRICXX_CHECK(delegate2 != delegate3);
	}
	
	// 3.4 non-static member function
	{
		NoexceptClassAddInt noexceptClassAddInt;
		
		Delegate<int (int, int) noexcept> delegate;
		delegate.Bind<&NoexceptClassAddInt::Add>(noexceptClassAddInt);
		
		Delegate<int (int, int) noexcept> delegate2 = delegate;
		BRICXX_CHECK(delegate2.IsBinded());
		bool noexceptClassAddIntIsBindedTo = delegate2.IsBindedTo<&NoexceptClassAddInt::Add>(noexceptClassAddInt);
		BRICXX_CHECK(noexceptClassAddIntIsBindedTo);
		BRICXX_CHECK(delegate2(1, 2) == 3);
		BRICXX_CHECK(noexceptClassAddInt.GetNumCalls() == 1U);
		
		Delegate<int (int, int) noexcept> delegate3;
		delegate3 = delegate;
		BRICXX_CHECK(delegate3.IsBinded());
		noexceptClassAddIntIsBindedTo = delegate3.IsBindedTo<&NoexceptClassAddInt::Add>(noexceptClassAddInt);
		BRICXX_CHECK(noexceptClassAddIntIsBindedTo);
		BRICXX_CHECK(delegate3(2, 3) == 5);
		BRICXX_CHECK(noexceptClassAddInt.GetNumCalls() == 2U);
		
		BRICXX_CHECK(delegate2 == delegate3);
		delegate3.Unbind();
		BRICXX_CHECK(delegate2 != delegate3);
	}
	
	
	//************************************************************
	// 4 noexcept-correctness
	
	// 4.1 (static member) function
	{
		Delegate<int (int, int) noexcept> delegate;
		//delegate.Bind<&FunctionAddInt>();	// OK, does not compile
		(void)delegate;
	}
	
	// 4.2 functor
	{
		FunctorAddInt functorAddInt;
		
		Delegate<int (int, int) noexcept> delegate;
		//delegate.Bind(functorAddInt);	// OK, does not compile
		(void)functorAddInt;
		(void)delegate;
	}
	
	// 4.3 lambda
	{
		unsigned int numCalls = 0U;
		auto lambdaAddInt = [&numCalls](int x, int y) mutable -> int {
			++numCalls;
			return x + y;
		};
		
		Delegate<int (int, int) noexcept> delegate;
		//delegate.Bind(lambdaAddInt);	// OK, does not compile
		(void)lambdaAddInt;
		(void)delegate;
	}
	
	// 4.4 non-static member function
	{
		ClassAddInt classAddInt;
		
		Delegate<int (int, int) noexcept> delegate;
		//delegate.Bind<&ClassAddInt::Add>(classAddInt);	// OK, does not compile
		(void)classAddInt;
		(void)delegate;
	}
	
	
	//************************************************************
	// 5 compatible signatures
	
	// 5.1 (static member) function
	{
		Delegate<int (int, int) noexcept> delegate;
		delegate.Bind<&NoexceptFunctionAddLong>();
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo<&NoexceptFunctionAddLong>());
		BRICXX_CHECK(delegate(1, 2) == 3);
	}
	
	// 5.2 functor
	{
		NoexceptFunctorAddLong noexceptFunctorAddLong;
		
		Delegate<int (int, int) noexcept> delegate;
		delegate.Bind(noexceptFunctorAddLong);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(noexceptFunctorAddLong));
		BRICXX_CHECK(delegate(1, 2) == 3);
		BRICXX_CHECK(noexceptFunctorAddLong.GetNumCalls() == 1U);
	}
	
	// 5.3 lambda
	{
		unsigned int numCalls = 0U;
		auto noexceptLambdaAddLong = [&numCalls](long x, long y) mutable noexcept -> long {
			++numCalls;
			return x + y;
		};
		
		Delegate<int (int, int) noexcept> delegate;
		delegate.Bind(noexceptLambdaAddLong);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(noexceptLambdaAddLong));
		BRICXX_CHECK(delegate(1, 2) == 3);
		BRICXX_CHECK(numCalls == 1U);
	}
	
	// 5.4 non-static member function
	{
		NoexceptClassAddLong noexceptClassAddLong;
		
		Delegate<int (int, int) noexcept> delegate;
		delegate.Bind<&NoexceptClassAddLong::Add>(noexceptClassAddLong);
		BRICXX_CHECK(delegate.IsBinded());
		bool noexceptClassAddLongIsBindedTo = delegate.IsBindedTo<&NoexceptClassAddLong::Add>(noexceptClassAddLong);
		BRICXX_CHECK(noexceptClassAddLongIsBindedTo);
		BRICXX_CHECK(delegate(1, 2) == 3);
		BRICXX_CHECK(noexceptClassAddLong.GetNumCalls() == 1U);
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
	
	// 2.1 (static member) function
	{
		Delegate<int (int, int) const noexcept> delegate;
		delegate.Bind<&NoexceptFunctionAddInt>();
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo<&NoexceptFunctionAddInt>());
		BRICXX_CHECK(delegate(1, 2) == 3);
		delegate.Unbind();
		BRICXX_CHECK(!delegate.IsBinded());
		
		Delegate delegate1 = Delegate<int (int, int) const noexcept>::CreateAndBind<&NoexceptFunctionAddInt>();
		BRICXX_CHECK(delegate1.IsBinded());
		BRICXX_CHECK(delegate1.IsBindedTo<&NoexceptFunctionAddInt>());
		BRICXX_CHECK(delegate1(1, 2) == 3);
	}
	
	// 2.2 functor
	{
		ConstNoexceptFunctorAddInt constNoexceptFunctorAddInt;
		
		Delegate<int (int, int) const noexcept> delegate;
		delegate.Bind(constNoexceptFunctorAddInt);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(constNoexceptFunctorAddInt));
		BRICXX_CHECK(delegate(1, 2) == 3);
		delegate.Unbind();
		BRICXX_CHECK(!delegate.IsBinded());
		
		Delegate delegate1 = Delegate<int (int, int) const noexcept>::CreateAndBind(constNoexceptFunctorAddInt);
		BRICXX_CHECK(delegate1.IsBinded());
		BRICXX_CHECK(delegate1.IsBindedTo(constNoexceptFunctorAddInt));
		BRICXX_CHECK(delegate1(1, 2) == 3);
	}
	
	// 2.3 lambda
	{
		auto constNoexceptLambdaAddInt = [](int x, int y) noexcept -> int {
			return x + y;
		};
		
		Delegate<int (int, int) const noexcept> delegate;
		delegate.Bind(constNoexceptLambdaAddInt);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(constNoexceptLambdaAddInt));
		BRICXX_CHECK(delegate(1, 2) == 3);
		delegate.Unbind();
		BRICXX_CHECK(!delegate.IsBinded());
		
		Delegate delegate1 = Delegate<int (int, int) const noexcept>::CreateAndBind(constNoexceptLambdaAddInt);
		BRICXX_CHECK(delegate1.IsBinded());
		BRICXX_CHECK(delegate1.IsBindedTo(constNoexceptLambdaAddInt));
		BRICXX_CHECK(delegate1(1, 2) == 3);
	}
	
	// 2.4 non-static member function
	{
		ConstNoexceptClassAddInt constNoexceptClassAddInt;
		
		Delegate<int (int, int) const noexcept> delegate;
		delegate.Bind<&ConstNoexceptClassAddInt::Add>(constNoexceptClassAddInt);
		BRICXX_CHECK(delegate.IsBinded());
		bool constNoexceptClassAddIntIsBindedTo = delegate.IsBindedTo<&ConstNoexceptClassAddInt::Add>(
			constNoexceptClassAddInt);
		BRICXX_CHECK(constNoexceptClassAddIntIsBindedTo);
		BRICXX_CHECK(delegate(1, 2) == 3);
		delegate.Unbind();
		BRICXX_CHECK(!delegate.IsBinded());
		
		Delegate delegate1 = Delegate<int (int, int) const noexcept>::CreateAndBind<&ConstNoexceptClassAddInt::Add>(
			constNoexceptClassAddInt);
		BRICXX_CHECK(delegate1.IsBinded());
		constNoexceptClassAddIntIsBindedTo = delegate1.IsBindedTo<&ConstNoexceptClassAddInt::Add>(
			constNoexceptClassAddInt);
		BRICXX_CHECK(constNoexceptClassAddIntIsBindedTo);
		BRICXX_CHECK(delegate1(1, 2) == 3);
	}
	
	
	//************************************************************
	// 3 copy constructor/assignment, equality
	
	// 3.1 (static member) function
	{
		Delegate<int (int, int) const noexcept> delegate;
		delegate.Bind<&NoexceptFunctionAddInt>();
		
		Delegate<int (int, int) const noexcept> delegate2 = delegate;
		BRICXX_CHECK(delegate2.IsBinded());
		BRICXX_CHECK(delegate2.IsBindedTo<&NoexceptFunctionAddInt>());
		BRICXX_CHECK(delegate2(1, 2) == 3);
		
		Delegate<int (int, int) const noexcept> delegate3;
		delegate3 = delegate;
		BRICXX_CHECK(delegate3.IsBinded());
		BRICXX_CHECK(delegate3.IsBindedTo<&NoexceptFunctionAddInt>());
		BRICXX_CHECK(delegate3(2, 3) == 5);
		
		BRICXX_CHECK(delegate2 == delegate3);
		delegate3.Unbind();
		BRICXX_CHECK(delegate2 != delegate3);
	}
	
	// 3.2 functor
	{
		ConstNoexceptFunctorAddInt constNoexceptFunctorAddInt;
		
		Delegate<int (int, int) const noexcept> delegate;
		delegate.Bind(constNoexceptFunctorAddInt);
		
		Delegate<int (int, int) const noexcept> delegate2 = delegate;
		BRICXX_CHECK(delegate2.IsBinded());
		BRICXX_CHECK(delegate2.IsBindedTo(constNoexceptFunctorAddInt));
		BRICXX_CHECK(delegate2(1, 2) == 3);
		
		Delegate<int (int, int) const noexcept> delegate3;
		delegate3 = delegate;
		BRICXX_CHECK(delegate3.IsBinded());
		BRICXX_CHECK(delegate3.IsBindedTo(constNoexceptFunctorAddInt));
		BRICXX_CHECK(delegate3(2, 3) == 5);
		
		BRICXX_CHECK(delegate2 == delegate3);
		delegate3.Unbind();
		BRICXX_CHECK(delegate2 != delegate3);
	}
	
	// 3.3 lambda
	{
		auto constNoexceptLambdaAddInt = [](int x, int y) noexcept -> int {
			return x + y;
		};
		
		Delegate<int (int, int) const noexcept> delegate;
		delegate.Bind(constNoexceptLambdaAddInt);
		
		Delegate<int (int, int) const noexcept> delegate2 = delegate;
		BRICXX_CHECK(delegate2.IsBinded());
		BRICXX_CHECK(delegate2.IsBindedTo(constNoexceptLambdaAddInt));
		BRICXX_CHECK(delegate2(1, 2) == 3);
		
		Delegate<int (int, int) const noexcept> delegate3;
		delegate3 = delegate;
		BRICXX_CHECK(delegate3.IsBinded());
		BRICXX_CHECK(delegate3.IsBindedTo(constNoexceptLambdaAddInt));
		BRICXX_CHECK(delegate3(2, 3) == 5);
		
		BRICXX_CHECK(delegate2 == delegate3);
		delegate3.Unbind();
		BRICXX_CHECK(delegate2 != delegate3);
	}
	
	// 3.4 non-static member function
	{
		ConstNoexceptClassAddInt constNoexceptClassAddInt;
		
		Delegate<int (int, int) const noexcept> delegate;
		delegate.Bind<&ConstNoexceptClassAddInt::Add>(constNoexceptClassAddInt);
		
		Delegate<int (int, int) const noexcept> delegate2 = delegate;
		BRICXX_CHECK(delegate2.IsBinded());
		bool constNoexceptClassAddIntIsBindedTo = delegate2.IsBindedTo<&ConstNoexceptClassAddInt::Add>(
			constNoexceptClassAddInt);
		BRICXX_CHECK(constNoexceptClassAddIntIsBindedTo);
		BRICXX_CHECK(delegate2(1, 2) == 3);
		
		Delegate<int (int, int) const noexcept> delegate3;
		delegate3 = delegate;
		BRICXX_CHECK(delegate3.IsBinded());
		constNoexceptClassAddIntIsBindedTo = delegate3.IsBindedTo<&ConstNoexceptClassAddInt::Add>(
			constNoexceptClassAddInt);
		BRICXX_CHECK(constNoexceptClassAddIntIsBindedTo);
		BRICXX_CHECK(delegate3(2, 3) == 5);
		
		BRICXX_CHECK(delegate2 == delegate3);
		delegate3.Unbind();
		BRICXX_CHECK(delegate2 != delegate3);
	}
	
	
	//************************************************************
	// 4 const-noexcept-correctness
	
	// 4.1 (static member) function
	{
		Delegate<int (int, int) const noexcept> delegate;
		//delegate.Bind<&FunctionAddInt>();	// OK, does not compile
		(void)delegate;
	}
	
	// 4.2 functor
	{
		Delegate<int (int, int) const noexcept> delegate;
		
		FunctorAddInt functorAddInt;
		//delegate.Bind(functorAddInt);	// OK, does not compile
		(void)functorAddInt;
		
		ConstFunctorAddInt constFunctorAddInt;
		//delegate.Bind(constFunctorAddInt);	// OK, does not compile
		(void)constFunctorAddInt;
		
		NoexceptFunctorAddInt noexceptFunctorAddInt;
		//delegate.Bind(noexceptFunctorAddInt);	// OK, does not compile
		(void)noexceptFunctorAddInt;
		
		(void)delegate;
	}
	
	// 4.3 lambda
	{
		Delegate<int (int, int) const noexcept> delegate;
		
		unsigned int numCalls = 0U;
		auto lambdaAddInt = [&numCalls](int x, int y) mutable -> int {
			++numCalls;
			return x + y;
		};
		//delegate.Bind(lambdaAddInt);	// OK, does not compile
		(void)lambdaAddInt;
		
		auto constLambdaAddInt = [](int x, int y) -> int {
			return x + y;
		};
		//delegate.Bind(constLambdaAddInt);	// OK, does not compile
		(void)constLambdaAddInt;
		
		auto noexceptLambdaAddInt = [&numCalls](int x, int y) mutable noexcept -> int {
			++numCalls;
			return x + y;
		};
		//delegate.Bind(noexceptLambdaAddInt);	// OK, does not compile
		(void)noexceptLambdaAddInt;
		
		(void)delegate;
	}
	
	// 4.4 non-static member function
	{
		Delegate<int (int, int) const noexcept> delegate;
		
		ClassAddInt classAddInt;
		//delegate.Bind<&ClassAddInt::Add>(classAddInt);	// OK, does not compile
		(void)classAddInt;
		
		ConstClassAddInt constClassAddInt;
		//delegate.Bind<&ConstClassAddInt::Add>(constClassAddInt);	// OK, does not compile
		(void)constClassAddInt;
		
		NoexceptClassAddInt noexceptClassAddInt;
		//delegate.Bind<&NoexceptClassAddInt::Add>(noexceptClassAddInt);	// OK, does not compile
		(void)noexceptClassAddInt;
		
		(void)delegate;
	}
	
	
	//************************************************************
	// 5 compatible signatures
	
	// 5.1 (static member) function
	{
		Delegate<int (int, int) const noexcept> delegate;
		delegate.Bind<&NoexceptFunctionAddLong>();
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo<&NoexceptFunctionAddLong>());
		BRICXX_CHECK(delegate(1, 2) == 3);
	}
	
	// 5.2 functor
	{
		ConstNoexceptFunctorAddLong constNoexceptFunctorAddLong;
		
		Delegate<int (int, int) const> delegate;
		delegate.Bind(constNoexceptFunctorAddLong);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(constNoexceptFunctorAddLong));
		BRICXX_CHECK(delegate(1, 2) == 3);
	}
	
	// 5.3 lambda
	{
		auto constNoexceptLambdaAddLong = [](long x, long y) noexcept -> long {
			return x + y;
		};
		
		Delegate<int (int, int) const> delegate;
		delegate.Bind(constNoexceptLambdaAddLong);
		BRICXX_CHECK(delegate.IsBinded());
		BRICXX_CHECK(delegate.IsBindedTo(constNoexceptLambdaAddLong));
		BRICXX_CHECK(delegate(1, 2) == 3);
	}
	
	// 5.4 non-static member function
	{
		ConstNoexceptClassAddLong constNoexceptClassAddLong;
		
		Delegate<int (int, int) const> delegate;
		delegate.Bind<&ConstNoexceptClassAddLong::Add>(constNoexceptClassAddLong);
		BRICXX_CHECK(delegate.IsBinded());
		bool constNoexceptClassAddLongIsBindedTo = delegate.IsBindedTo<&ConstNoexceptClassAddLong::Add>(
			constNoexceptClassAddLong);
		BRICXX_CHECK(constNoexceptClassAddLongIsBindedTo);
		BRICXX_CHECK(delegate(1, 2) == 3);
	}
}


inline int FunctionAddInt(int x, int y) {
	return x + y;
}

inline int FunctorAddInt::operator()(int x, int y) {
	++numCalls_;
	return x + y;
}

inline unsigned int FunctorAddInt::GetNumCalls() const {
	return numCalls_;
}

inline int ClassAddInt::Add(int x, int y) {
	++numCalls_;
	return x + y;
}

inline unsigned int ClassAddInt::GetNumCalls() const {
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

inline unsigned int NoexceptFunctorAddInt::GetNumCalls() const {
	return numCalls_;
}

inline int NoexceptClassAddInt::Add(int x, int y) noexcept {
	++numCalls_;
	return x + y;
}

inline unsigned int NoexceptClassAddInt::GetNumCalls() const {
	return numCalls_;
}


inline int ConstNoexceptFunctorAddInt::operator()(int x, int y) const noexcept {
	return x + y;
}

inline int ConstNoexceptClassAddInt::Add(int x, int y) const noexcept {
	return x + y;
}


inline long FunctionAddLong(long x, long y) {
	return x + y;
}

inline long FunctorAddLong::operator()(long x, long y) {
	++numCalls_;
	return x + y;
}

inline unsigned int FunctorAddLong::GetNumCalls() const {
	return numCalls_;
}

inline long ClassAddLong::Add(long x, long y) {
	++numCalls_;
	return x + y;
}

inline unsigned int ClassAddLong::GetNumCalls() const {
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

inline unsigned int NoexceptFunctorAddLong::GetNumCalls() const {
	return numCalls_;
}

inline long NoexceptClassAddLong::Add(long x, long y) noexcept {
	++numCalls_;
	return x + y;
}

inline unsigned int NoexceptClassAddLong::GetNumCalls() const {
	return numCalls_;
}


inline long ConstNoexceptFunctorAddLong::operator()(long x, long y) const noexcept {
	return x + y;
}

inline long ConstNoexceptClassAddLong::Add(long x, long y) const noexcept {
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
