tictacto

Going thru neural nets tutorial:

   https://medium.com/swlh/tic-tac-toe-and-deep-neural-networks-ea600bc53f51

Cobbled up tic-tac-to game program that uses a Qtable to play computers moves.
The resulting game player is ok, not great. The design is overkill, and underkill.
Was a learning experience.

   qtable.h, qtable.C          - Qstates class definitions

   tictacto_common.h           - some shared game enums
   
   ttt_games_gen.C             - code used to play random tic-tac-to games,
   tictacto_games_generator.C  -   and create games data in form of XML file.
   tictacto_games_generator.h  -     

   gen_qtables.C               - code used to read games data file, populate
   tictacto_qtable_generator.C - Qtables, and write out qtables XML file.
   tictacto_qtable_generator.h -

   qtables_play.C              - tic-tac-to game player. computer uses Qtable
   qtable.C                    - created previously to make moves
