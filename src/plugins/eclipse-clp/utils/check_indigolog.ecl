/***************************************************************************
 *  check_indigolog.ecl - Basic checker for IndiGolog
 *
 *  Created: Mon May 26 15:26:41 2014
 *  Copyright  2014  Gesche Gierse
 *
 ****************************************************************************/

/*  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  Read the full text in the LICENSE.GPL file in the doc directory.
 */


/* This checker will look if for every action named adequat
 * execute and poss predicates are defined and vise-versa. Same
 * method is applied to check that fluents are well defined, too.
 *
 * Usage: Load this module in the agent you want to check.
 * Copy the following predicates to the agent:

:- dynamic execute/2.
:- dynamic prim_action/1.
:- dynamic poss/2.
:- dynamic prim_fluent/1.
:- dynamic initially/2.
:- dynamic senses/2.
:- dynamic causes_val/4.
:- dynamic clause/2.

 * Call the predicate basic_check/0 somewhere at the start of your
 * agent (e.g. start of run/0 or in initialisation method).
 * Run fawkes with the readylogplugin and your agent configured.
 * -> There will be warnings if checker detected anything.
 * After usage comment out/delete the dynamic declarations from above,
 * otherwise IndiGolog will _not_ work!
*/

:- module(check_indigolog).

:- export basic_check/0.
:- tool(basic_check/0, basic_check/1).
:- use_module("logging").

:- dynamic(fluent/1).
:- dynamic(action/1).

%% examples to test functionality
/*
:- dynamic execute/2.
:- dynamic prim_action/1.
:- dynamic poss/2.
:- dynamic prim_fluent/1.
:- dynamic initially/2.
:- dynamic senses/2.
:- dynamic causes_val/4.
:- dynamic clause/2.

prim_action(drive(N)).
prim_action(say).

poss(drive, _).
poss(only_poss, _).
poss(only_exec, _).
poss(say, _).

execute(drive(N), _) :- writeln(N).
execute(only_exec, _) :- writeln("execute that action"), writeln(A).
execute(only_exec2,_).
*/
%prim_fluent(closer).
%prim_fluent(foo).
%initially(closer, _).
/*initially(bar,_).

causes_val(_, closer, _, _).
senses(_, closer).
senses(_, bar).
*/


basic_check(M) :-
    log_info("GologChecker: checking actions..."),
    debug_action(M),
    log_info("GologChecker: checking fluents..."),
    debug_fluent(M),
    log_info("GologChecker: basic check finished.").

debug_action(M) :- has_action(A, M), assert(action(A)), check_action(A,M), fail.
debug_action(M) :- has_execute(A, M), check_action2(A, M), fail.
debug_action(M) :- has_poss(A, M), check_action3(A, M), fail.

debug_action(M) :-
    list_all_actions(List),
    log_info("GologChecker: Found actions: %D_w", [List]),
    log_info("GologChecker: basic action check finished.").
%debug_action(M) :- log_info("GologChecker: Found Actions:"), fail.
%debug_action(M) :- action(A), write(A), write("\n"), fail.
%debug_action(M) :- log_info("GologChecker: basic action check finished.").


list_all_actions(Res) :- action(A), ground(A), list_all_actions2([], Res).
list_all_actions2(List, [A|Res]) :- action(A), ground(A), not_in_list(A, List), /*log_info("The current list is %w, the action is: %w", [List, A]), */!, list_all_actions2([A|List], Res).
list_all_actions2(List, [A|Res]) :- action(A), nonground(A, n), not_in_list(A, List), /*log_info("The current list is %w, the action is: %w", [List, A]),*/ !, list_all_actions2([A|List], Res).
list_all_actions2(_, []).

not_in_list(A, []).
not_in_list(A, [Head|Tail]) :- A \= Head, not_in_list(A, Tail).

% When prim_fluent(A) is found, check if execute and poss exist, too.
check_action(A,M) :-
	(has_execute(A, M)
		; !,
		  log_warn("GologChecker: WARNING: Action \"%w\" defined, but no execute() found!", [A])),
	(has_poss(A, M), ! ; !, log_warn("GologChecker: WARNING: Action \"%w\" defined, but no poss() found!", [A])).

% When execute(A, _) is found, check if prim and poss exist, too.
check_action2(A, M) :-
	(clause(action(A)), !
	;	!,
		assert(action(A)),
		(has_action(A, M)
			; !,
			  log_warn("GologChecker: WARNING: Execute action \"%w\" defined, but no prim_fluent() found!", [A])),
		(has_poss(A, M), ! ; !, log_warn("GologChecker: WARNING: Execute action \"%w\" defined, but no poss() found!", [A]))).

% When poss(A) is found, check if prim and execute exist, too.
check_action3(A, M) :-
	(clause(action(A)), !
	;	!,
		assert(action(A)),
		(has_action(A, M)
			; !,
			  log_warn("GologChecker: WARNING: Poss action \"%w\" defined, but no prim_fluent() found!",  [A])),
		(has_execute(A, M), ! ; !, log_warn("GologChecker: WARNING: Poss action \"%w\" defined, but no execute() found!", [A]))).

has_action(A, M) :- clause(prim_action(A), _body)@M.
has_execute(A, M) :- clause(execute(A,_), _body)@M.
has_poss(A, M) :- clause(poss(A,_))@M.


debug_fluent(M) :- has_fluent(F, M), assert(fluent(F)), check_fluent(F, M), fail.
debug_fluent(M) :- has_initially(F, M), check_fluent2(F, M), fail.
debug_fluent(M) :- has_senses(F, M), check_fluent3(F, M), fail.
debug_fluent(M) :- has_causes(F, M), check_fluent4(F, M), fail.
%debug_fluent(M) :- log_info("Found the following fluents: "), fail.
%debug_fluent(M) :- fluent(F), write(F), write(" "), fail.
debug_fluent(M) :-
    list_all_fluents(List),
    log_info("GologChecker: Found fluents: %D_w", [List]),
    log_info("GologChecker: basic fluent check finished.").


list_all_fluents(Res) :- fluent(A), ground(A), list_all_fluents2([], Res).
list_all_fluents2(List, [A|Res]) :- fluent(A), ground(A), not_in_list(A, List), !, list_all_fluents2([A|List], Res).
list_all_fluents2(List, [A|Res]) :- fluent(A), nonground(A, n), not_in_list(A, List), !, list_all_fluents2([A|List], Res).
list_all_fluents2(_, []).

% When prim_fluent found, check if initially and senses or causes_val exist.
check_fluent(F, M) :-
	(has_initially(F, M)
		; !,
		   log_warn("GologChecker: WARNING: Fluent \"%w\" defined, but no initially() found!", [F])),
	(((has_senses(F, M) ; has_causes(F, M)), !)
	   ; (!, log_warn("GologChecker: WARNING: Fluent \"%w\" defined, but neither senses nor causes_val found!", [A]))).

% When initially found, check if prim_fluent and senses or causes_val exist.
check_fluent2(F, M) :-
	(clause(fluent(F)), !
	;	!,
		assert(fluent(F)),
		(has_action(F, M)
			; !,
			   log_warn("GologChecker: WARNING: Fluent \"%w\" defined, but no initially() found!", [F])),
		(((has_senses(F, M) ; has_causes(F, M)), !)
		   ; (!, log_warn("GologChecker: WARNING: Fluent \"%w\" defined, but neither senses nor causes_val found!", [F])))).

% When senses found, check if initially and prim_fluent exist.
check_fluent3(F, M) :-
	(clause(fluent(F)), !
	;	!,
		assert(fluent(F)),
		(has_initially(F, M)
			; !,
			   log_warn("GologChecker: WARNING: \"senses(%w)\" defined, but no initially() found!", [A])),
		((has_fluent(F, M), !)
		   ; (!, log_warn("GologChecker: WARNING: \"senses(%w)\" defined, but no fluent found!", [A])))).

% When senses found, check if initially and prim_fluent exist.
check_fluent4(F, M) :-
	(clause(fluent(F)), !
	;	!,
		assert(fluent(F)),
		(has_initially(F, M)
			; !,
			   log_warn("GologChecker: WARNING: \"causes_val(%w)\" defined, but no initially() found!", [A])),
		((has_fluent(F, M), !)
		   ; (!, log_warn("GologChecker: WARNING: \"causes_val(%w)\" defined, but no fluent found!", [A])))).

has_fluent(F, M) :- clause(prim_fluent(F), _body)@M.
has_initially(F, M) :- clause(initially(F, _))@M.
has_senses(F, M) :- clause(senses(_, F))@M.
has_causes(F, M) :- clause(causes_val(_, F, _, _))@M.


subv(_,_,T1,T2) :- (var(T1);integer(T1)), !, T2 = T1.
subv(X1,X2,T1,T2) :- T1 = X1, !, T2 = X2.
subv(X1,X2,T1,T2) :- T1 =..[F|L1], subvl(X1,X2,L1,L2), T2 =..[F|L2].
subvl(_,_,[],[]).
subvl(X1,X2,[T1|L1],[T2|L2]) :- subv(X1,X2,T1,T2), subvl(X1,X2,L1,L2).
