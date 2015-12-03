type token =
  | NL
  | REMOPEN
  | REMCLOSE
  | NEVER
  | IF
  | FI
  | GOTO
  | SKIP
  | COLON
  | SEMICOLON
  | DCOLON
  | LBRACK
  | RBRACK
  | RARROW
  | LABEL of (string)
  | ID of (string)
  | NUM of (int)

open Parsing;;
# 21 "neverp.mly"
open String
open Declarations
open Mutils
open Putils

(* Not really a conversion but rather this: "T0_S1" -> "01" *)

let rec string_to_int s =
  try
    ignore (extfind s "accept_all");
    "666"  (* 666 is final state *)
  with _ ->
    try
      ignore (extfind s "accept_init");
      "0"  (* 666 is final state *)
    with _ ->
      if (String.length s >= 1) then (
	if not(isint s.[0]) then 
	  string_to_int (rmchar s s.[0])
	else
	  (string_of_char (s.[0])) ^ (string_to_int (lchop s))
      )
      else ""

 let last_state = ref ""
# 48 "neverp.ml"
let yytransl_const = [|
  257 (* NL *);
  258 (* REMOPEN *);
  259 (* REMCLOSE *);
  260 (* NEVER *);
  261 (* IF *);
  262 (* FI *);
  263 (* GOTO *);
  264 (* SKIP *);
  265 (* COLON *);
  266 (* SEMICOLON *);
  267 (* DCOLON *);
  268 (* LBRACK *);
  269 (* RBRACK *);
  270 (* RARROW *);
    0|]

let yytransl_block = [|
  271 (* LABEL *);
  272 (* ID *);
  273 (* NUM *);
    0|]

let yylhs = "\255\255\
\001\000\001\000\002\000\003\000\004\000\004\000\005\000\005\000\
\005\000\005\000\009\000\007\000\007\000\006\000\008\000\000\000"

let yylen = "\002\000\
\000\000\002\000\002\000\003\000\000\000\002\000\001\000\001\000\
\001\000\001\000\001\000\001\000\002\000\002\000\005\000\002\000"

let yydefred = "\000\000\
\001\000\000\000\000\000\000\000\002\000\005\000\003\000\000\000\
\012\000\000\000\011\000\000\000\004\000\000\000\006\000\007\000\
\008\000\009\000\010\000\013\000\000\000\014\000\000\000\000\000\
\015\000"

let yydgoto = "\002\000\
\003\000\005\000\007\000\008\000\015\000\016\000\017\000\018\000\
\019\000"

let yysindex = "\001\000\
\000\000\000\000\000\255\249\254\000\000\000\000\000\000\251\254\
\000\000\253\254\000\000\250\254\000\000\001\255\000\000\000\000\
\000\000\000\000\000\000\000\000\254\254\000\000\006\255\255\254\
\000\000"

let yyrindex = "\000\000\
\000\000\000\000\014\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000"

let yygindex = "\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000"

let yytablesize = 15
let yytable = "\009\000\
\010\000\001\000\011\000\004\000\006\000\012\000\020\000\013\000\
\021\000\022\000\014\000\023\000\024\000\016\000\025\000"

let yycheck = "\005\001\
\006\001\001\000\008\001\004\001\012\001\011\001\010\001\013\001\
\015\001\009\001\016\001\014\001\007\001\000\000\016\001"

let yynames_const = "\
  NL\000\
  REMOPEN\000\
  REMCLOSE\000\
  NEVER\000\
  IF\000\
  FI\000\
  GOTO\000\
  SKIP\000\
  COLON\000\
  SEMICOLON\000\
  DCOLON\000\
  LBRACK\000\
  RBRACK\000\
  RARROW\000\
  "

let yynames_block = "\
  LABEL\000\
  ID\000\
  NUM\000\
  "

let yyact = [|
  (fun _ -> failwith "parser")
; (fun __caml_parser_env ->
    Obj.repr(
# 59 "neverp.mly"
                        ( )
# 144 "neverp.ml"
               : unit))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 1 : unit) in
    let _2 = (Parsing.peek_val __caml_parser_env 0 : 'never) in
    Obj.repr(
# 60 "neverp.mly"
                        ( )
# 152 "neverp.ml"
               : unit))
; (fun __caml_parser_env ->
    let _2 = (Parsing.peek_val __caml_parser_env 0 : 'claim) in
    Obj.repr(
# 63 "neverp.mly"
                        ( )
# 159 "neverp.ml"
               : 'never))
; (fun __caml_parser_env ->
    let _2 = (Parsing.peek_val __caml_parser_env 1 : 'statement_list) in
    Obj.repr(
# 66 "neverp.mly"
                                         ( )
# 166 "neverp.ml"
               : 'claim))
; (fun __caml_parser_env ->
    Obj.repr(
# 68 "neverp.mly"
                                 ( )
# 172 "neverp.ml"
               : 'statement_list))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 1 : 'statement_list) in
    let _2 = (Parsing.peek_val __caml_parser_env 0 : 'statement) in
    Obj.repr(
# 69 "neverp.mly"
                                             ( )
# 180 "neverp.ml"
               : 'statement_list))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 0 : 'state) in
    Obj.repr(
# 73 "neverp.mly"
                 ( )
# 187 "neverp.ml"
               : 'statement))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 0 : 'iff) in
    Obj.repr(
# 74 "neverp.mly"
                 ( )
# 194 "neverp.ml"
               : 'statement))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 0 : 'transition) in
    Obj.repr(
# 75 "neverp.mly"
                 ( )
# 201 "neverp.ml"
               : 'statement))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 0 : 'skip) in
    Obj.repr(
# 76 "neverp.mly"
                 ( )
# 208 "neverp.ml"
               : 'statement))
; (fun __caml_parser_env ->
    Obj.repr(
# 80 "neverp.mly"
            ( 
	      Declarations.transitions := !Declarations.transitions @
		["666", "(1)", "666"]
	    )
# 217 "neverp.ml"
               : 'skip))
; (fun __caml_parser_env ->
    Obj.repr(
# 85 "neverp.mly"
                         ( )
# 223 "neverp.ml"
               : 'iff))
; (fun __caml_parser_env ->
    Obj.repr(
# 86 "neverp.mly"
                         ( )
# 229 "neverp.ml"
               : 'iff))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 1 : string) in
    Obj.repr(
# 90 "neverp.mly"
            ( 
	      last_state := _1;
	      try 
		ignore (extfind _1 "accept");
		Declarations.states := !Declarations.states @ 
		  [(string_to_int _1)]
	      with Invalid_string -> Printf.printf ("") 
	    )
# 243 "neverp.ml"
               : 'state))
; (fun __caml_parser_env ->
    let _2 = (Parsing.peek_val __caml_parser_env 3 : string) in
    let _5 = (Parsing.peek_val __caml_parser_env 0 : string) in
    Obj.repr(
# 102 "neverp.mly"
            ( 
	      try
		ignore (extfind (rmchar _2 ' ') "(1)");
		Declarations.transitions := !Declarations.transitions @  
			[(string_to_int !last_state), 
			 "(1)", 
			 (string_to_int _5)]
	      with _ ->
		let labels = ref (break_label _2) in
		  while (!labels != []) do
		    let h = List.hd !labels in
		      Declarations.transitions := !Declarations.transitions @  
			[(string_to_int !last_state), 
			 (rmchar h ' '), 
			 (string_to_int _5)] ;
		      labels := List.tl !labels
		  done
	    )
# 268 "neverp.ml"
               : 'transition))
(* Entry input *)
; (fun __caml_parser_env -> raise (Parsing.YYexit (Parsing.peek_val __caml_parser_env 0)))
|]
let yytables =
  { Parsing.actions=yyact;
    Parsing.transl_const=yytransl_const;
    Parsing.transl_block=yytransl_block;
    Parsing.lhs=yylhs;
    Parsing.len=yylen;
    Parsing.defred=yydefred;
    Parsing.dgoto=yydgoto;
    Parsing.sindex=yysindex;
    Parsing.rindex=yyrindex;
    Parsing.gindex=yygindex;
    Parsing.tablesize=yytablesize;
    Parsing.table=yytable;
    Parsing.check=yycheck;
    Parsing.error_function=parse_error;
    Parsing.names_const=yynames_const;
    Parsing.names_block=yynames_block }
let input (lexfun : Lexing.lexbuf -> token) (lexbuf : Lexing.lexbuf) =
   (Parsing.yyparse yytables 1 lexfun lexbuf : unit)
;;
# 122 "neverp.mly"

# 295 "neverp.ml"
