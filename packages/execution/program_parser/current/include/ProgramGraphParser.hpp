/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  ProgramGraphParser.hpp

                        ProgramGraphParser.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#ifndef EXECUTION_PROGRAM_PARSER_HPP
#define EXECUTION_PROGRAM_PARSER_HPP

#include "parser-types.hpp"

#include "CommonParser.hpp"
#include "ConditionParser.hpp"
#include "CommandParser.hpp"
#include "ExpressionParser.hpp"
#include "TaskContext.hpp"
#include "ValueChangeParser.hpp"
#include "ProgramGraph.hpp"

#include <map>
#include <vector>
#include <string>

namespace ORO_Execution { namespace detail
{
  /**
   * @brief A Parser for Orocos Program Scripts.
   *
   * This class does the actual work.  It generates a Program, by
   * constructing the ProgramNode's.  We just go over the code one
   * single time, and construct the Program as we go.  For every
   * statement, we construct a new ProgramNode, and fill it up as we
   * get the information we need.
   */
  class ProgramGraphParser
  {
      typedef ProgramGraph::CommandNode CommandNode;
      typedef ProgramGraph::ConditionEdge ConditionEdge;
      typedef ProgramGraph::Graph  Graph;

      /**
       * The context given to us by the user to root 
       * the parsed programs into.
       */
      TaskContext* rootc;

      /**
       * The current subcontext of root where we are working in.
       */
      TaskContext* context;

      /**
       * The current function context where we are working in.
       */
      TaskContext* fcontext;

      our_pos_iter_t& mpositer;

      // helper variable to detect if we are in 'try' or 'do'
      // which influences the 'and' behaviour
      bool try_cmd;

      // The current function we are parsing.
      FunctionGraph* mfunc;

      // The function we will call next
      FunctionGraph* mcallfunc;

      // A map of all functions
      typedef std::map<std::string, FunctionGraph*> funcmap;
      funcmap mfuncs;

      // the label that the user wants to give to the current
      // statement.  If this is empty, then no label is necessary..
      std::string mcurlabel;

      // The implicit termination condition we just got for the
      // current command from the CommandParser..
      ConditionInterface* implcond;
      std::vector<ConditionInterface*> implcond_v;
      std::vector<DataSourceBase::shared_ptr> callfnargs;

      // last seen condition
      ConditionInterface* mcondition;
      // try-block condition
      ConditionInterface* try_cond;

      // Dispatch Condition
      ConditionInterface* dc;

      void seencondition();

      void seencallfunclabel( iter_t begin, iter_t end );

      void seencontinue( );

      void startofnewstatement( const std::string& type);
      void seencommandcall();
      void seenandcall();
      void seencommands();
      void seendostatement();
      void seentrystatement();
      void startcatchpart();
      void seencatchpart();

      void seenvaluechange();

      void functiondef( iter_t begin, iter_t end );
      void exportdef(  );
      void seenfunctionarg();
      void seenfunctionend();

      void seenfuncidentifier( iter_t begin, iter_t end);
      void seencallfuncstatement();
      void seencallfuncargs();

      void seenreturnstatement();
      void seenreturnlabel();

      void seenifstatement();
      void endifblock();
      void endifstatement();

      void seenwhilestatement();
      void endwhilestatement();

      void seenbreakstatement();

      void seenforstatement();
      void seenforinit();
      void seenforincr();
      void endforstatement();

      void startofprogram();
      void programdef( iter_t begin, iter_t end );
      void seenprogramend();
      void programtext(iter_t, iter_t);

      void ProgramGraphParser::setStack(TaskContext* st);
      void cleanup();

      rule_t newline, terminationclause, jumpdestination, terminationpart, andpart,
          dostatement, trystatement, statement, line, content, program, valuechange_parsers,
          production, valuechange, returnstatement, function, functions, arguments, funcstatement,
          continuepart, returnpart, callpart, ifstatement, ifblock, whilestatement, breakstatement,
          openbrace, closebrace, opencurly, closecurly, forstatement, semicolon,
          condition, catchpart, funcargs, functionarg ;

      ConditionParser conditionparser;
      CommonParser commonparser;
      CommandParser commandparser;
      ValueChangeParser valuechangeparser;
      ExpressionParser expressionparser;
      ArgumentsParser* argsparser;

      ProgramGraph* program_graph;
      std::vector<ProgramGraph*> program_list;

      CommandInterface* for_init_command;
      CommandInterface* for_incr_command;
      std::string program_text;
      bool exportf;
      int ln_offset;
  public:
      ProgramGraphParser( iter_t& positer, TaskContext*);

      /**
       * @brief Tries to parse programs, returns the generated programs on success.
       * @throw file_parse_exception The parser found an error.
       */
      std::vector<ProgramGraph*> parse( iter_t& begin, iter_t end );

      std::vector<FunctionGraph*> parseFunction( iter_t& begin, iter_t end );

      void initBodyParser(const std::string& name, TaskContext* stck, int offset);
      rule_t& bodyParser();
      FunctionGraph* bodyParserResult();

  };
}}

#endif
