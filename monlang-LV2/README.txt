=== ERROR CODE CONVENTION =================================

--- Error code pattern ------------------------------------

11 Program
12 Statement
16 Expression

~~~

21 Assignment
22 Accumulation

23 LetStatement
24 VarStatement

25 ReturnStatement
26 BreakStatement
27 ContinueStatement
28 DieStatement

29 Guard
31 IfStatement

32 ForeachStatement
33 WhileStatement

34 DeferStatement

59 ExpressionStatement

~~~

61 Operation

62 FunctionCall
63 Lambda
64 BlockExpression
65 Literal
66 SpecialSymbol

99 Lvalue

--- Manual of error codes ---------------------------------

111 Malformed Program, contains a malformed statement

161 Malformed Expression, contains an even number of words
162 Malformed Expression, contains a non-Atom word as operator
163 Malformed Expression, contains an unknown operator (unfound in precedence table)
169 Unknown Expression
