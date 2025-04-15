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
34 DoWhileStatement
35 C_DoStatement
36 C_WhileStatement

37 DeferStatement

59 ExpressionStatement

~~~

61 Operation

62 FunctionCall
63 Lambda
64 BlockExpression
65 Numeral
66 SpecialSymbol
67 StrLiteral
68 ListLiteral
69 MapLiteral
71 FieldAccess
72 Subscript
73 FunctionCallArgument

99 Lvalue

--- Manual of error codes ---------------------------------

111 Malformed Program, contains a Malformed Statement

121 Unknown Statement

161 Malformed Expression, contains an even number of words
162 Malformed Expression, contains a non-Atom word as operator
  -> not triggerable when calling consumeProgram(LV1::Program&),
     only triggerable when calling buildExpression(Term)
163 Malformed Expression, contains an unknown operator (unfound in precedence table)
169 Unknown Expression

~~~

211 Malformed Assignment, contains a non-Word as variable
212 Malformed Assignment, contains a non-Lvalue expression as variable
216 Malformed Assignment, contains a Malformed Lvalue as variable
213 Malformed Assignment, contains less than 3 words (no value)
214 Malformed Assignment, contains a non-Word as part of the value
215 Malformed Assignment, contains a Malformed Expression as value

226 Malformed Accumulation, contains an unknown operator (unfound in precedence table)
221 Malformed Accumulation, contains a non-Word as variable
222 Malformed Accumulation, contains a non-Lvalue expression as variable
227 Malformed Accumulation, contains a Malformed Lvalue as variable
223 Malformed Accumulation, contains less than 3 words (no value)
224 Malformed Accumulation, contains a non-Word as part of the value
225 Malformed Accumulation, contains a Malformed Expression as value

231 Malformed LetStatement, contains less than 2 words (no name)
236 Malformed LetStatement, contains a non-Word as name
232 Malformed LetStatement, contains a non-Symbol as name
233 Malformed LetStatement, contains less than 3 words (no value)
234 Malformed LetStatement, contains a non-Word as part of the value
235 Malformed LetStatement, contains a Malformed Expression as value

241 Malformed VarStatement, contains less than 2 words (no name)
246 Malformed VarStatement, contains a non-Word as name
242 Malformed VarStatement, contains a non-Symbol as name
243 Malformed VarStatement, contains less than 3 words (no value)
244 Malformed VarStatement, contains a non-Word as part of the value
245 Malformed VarStatement, contains a Malformed Expression as value

251 Malformed ReturnStatement, contains a non-Word as part of the value
252 Malformed ReturnStatement, contains a Malformed Expression as value

321 Malformed ForeachStatement, contains less than 3 words (no iterable AND/OR no block)
322 Malformed ForeachStatement, contains a non-Word as part of iterable
323 Malformed ForeachStatement, contains a Malformed Expression as iterable
324 Malformed ForeachStatement, contains a non-Word as block
326 Malformed ForeachStatement, contains a non-BlockExpression as block
328 Malformed ForeachStatement, contains a dollars BlockExpression as block
327 Malformed ForeachStatement, contains a oneline BlockExpression as block
325 Malformed ForeachStatement, contains a Malformed BlockExpression as block

331 Malformed WhileStatement, contains less than 2 words (no condition)
332 Malformed WhileStatement, contains a non-SquareBracketsTerm as condition
333 Malformed WhileStatement, contains a Malformed Expression as part of condition
334 Malformed WhileStatement, contains less than 3 words (no block)
335 Malformed WhileStatement, contains a non-Word as block
338 Malformed WhileStatement, contains a non-BlockExpression as block
337 Malformed WhileStatement, contains a dollars BlockExpression as block
339 Malformed WhileStatement, contains a oneline BlockExpression as block
336 Malformed WhileStatement, contains a Malformed BlockExpression as block

341 Malformed DoWhileStatement, contains a Malformed C_DoStatement
342 Malformed DoWhileStatement, missing C_WhileStatement
343 Malformed DoWhileStatement, contains a Malformed C_WhileStatement

351 Malformed C_DoStatement, contains less than 2 words (no block)
352 Malformed C_DoStatement, contains a non-Word as block
353 Malformed C_DoStatement, contains a non-BlockExpression as block
356 Malformed C_DoStatement, contains a dollars BlockExpression as block
355 Malformed C_DoStatement, contains a oneline BlockExpression as block
354 Malformed C_DoStatement, contains a Malformed BlockExpression as block

361 Malformed C_WhileStatement, contains less than 2 words (no condition)
362 Malformed C_WhileStatement, contains a non-SquareBracketsTerm as condition
363 Malformed C_WhileStatement, contains a Malformed Expression as part of condition

591 Malformed ExpressionStatement, contains a Malformed Expression

~~~

611 Malformed Operation, contains a Malformed Expression as left operand
612 Malformed Operation, contains a Malformed Expression as right operand

621 Malformed FunctionCall, contains a Malformed Expression as function
622 Malformed FunctionCall, contains a Malformed FunctionCallArgument

731 Malformed FunctionCallArgument, contains a Malformed Expression
732 Malformed FunctionCallArgument, contains a non-lvalue passed by reference

631 Malformed Lambda, contains a Malformed Statement within body

641 Malformed BlockExpression, contains a Malformed Statement

681 Malformed ListLiteral, contains a malformed Expression

691 Malformed MapLiteral, contains a List argument
692 Malformed MapLiteral, contains a malformed Expression as argument key
693 Malformed MapLiteral, contains a malformed Expression as argument value

711 Malformed FieldAccess, contains a malformed Expression as object
712 Malformed FieldAccess, contains a non-Symbol as field

721 Malformed Subscript, contains a malformed Expression as array
722 Malformed Subscript, contains a malformed Expression as key
