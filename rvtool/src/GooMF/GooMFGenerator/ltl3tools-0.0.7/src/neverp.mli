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

val input :
  (Lexing.lexbuf  -> token) -> Lexing.lexbuf -> unit
