// Philipp Klaus Krause, philipp@informatik.uni-frankfurt.de, pkk@spth.de, 2010 - 2011
//
// (c) 2012 Goethe-Universität Frankfurt
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option) any
// later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//#define DEBUG_RALLOC_DEC // Uncomment to get debug messages while doing register allocation on the tree decomposition.
//#define DEBUG_RALLOC_DEC_ASS // Uncomment to get debug messages about assignments while doing register allocation on the tree decomposition (much more verbose than the one above).

#define TD_SALLOC
#define CH_SALLOC

#include "SDCCralloc.hpp"

extern "C"
{
  #include "ralloc.h"
  #include "gen.h"
  unsigned char dryhc08iCode (iCode *ic);
};

#define REG_A 0
#define REG_H 1
#define REG_X 2

template <class I_t>
static void add_operand_conflicts_in_node(const cfg_node &n, I_t &I)
{
  const iCode *ic = n.ic;
  
  const operand *result = IC_RESULT(ic);
  const operand *left = IC_LEFT(ic);
  const operand *right = IC_RIGHT(ic);
	
  if(!result || !IS_SYMOP(result))
    return;
    
  // Todo: Identify operations that code generation can always handle and exclude them (as done for the z80-like ports).
   
  operand_map_t::const_iterator oir, oir_end, oirs; 
  boost::tie(oir, oir_end) = n.operands.equal_range(OP_SYMBOL_CONST(result)->key);
  if(oir == oir_end)
    return;
    
  operand_map_t::const_iterator oio, oio_end;
  
  if(left && IS_SYMOP(left))
    for(boost::tie(oio, oio_end) = n.operands.equal_range(OP_SYMBOL_CONST(left)->key); oio != oio_end; ++oio)
      for(oirs = oir; oirs != oir_end; ++oirs)
        {
          var_t rvar = oirs->second;
          var_t ovar = oio->second;
          if(I[rvar].byte < I[ovar].byte)
            boost::add_edge(rvar, ovar, I);
        }
        
  if(right && IS_SYMOP(right))
    for(boost::tie(oio, oio_end) = n.operands.equal_range(OP_SYMBOL_CONST(right)->key); oio != oio_end; ++oio)
      for(oirs = oir; oirs != oir_end; ++oirs)
        {
          var_t rvar = oirs->second;
          var_t ovar = oio->second;
          if(I[rvar].byte < I[ovar].byte)
            boost::add_edge(rvar, ovar, I);
        }
}

// Return true, iff the operand is placed (partially) in r.
template <class G_t>
static bool operand_in_reg(const operand *o, reg_t r, const i_assignment_t &ia, unsigned short int i, const G_t &G)
{
  if(!o || !IS_SYMOP(o))
    return(false);

  operand_map_t::const_iterator oi, oi_end;
  for(boost::tie(oi, oi_end) = G[i].operands.equal_range(OP_SYMBOL_CONST(o)->key); oi != oi_end; ++oi)
    if(oi->second == ia.registers[r][1] || oi->second == ia.registers[r][0])
      return(true);

  return(false);
}

// Check that the operand is either fully in registers or fully in memory.
template <class G_t, class I_t>
static bool operand_sane(const operand *o, const assignment &a, unsigned short int i, const G_t &G, const I_t &I)
{
  if(!o || !IS_SYMOP(o))
    return(true);
 
  operand_map_t::const_iterator oi, oi2, oi_end;
  boost::tie(oi, oi_end) = G[i].operands.equal_range(OP_SYMBOL_CONST(o)->key);
  
  if(oi == oi_end)
    return(true);

  // Register combinations code generation cannot handle.
  if(a.local.find(oi->second) != a.local.end() && a.local.find((oi2 = oi, ++oi2)->second) != a.local.end())
    {
      const reg_t l =a.global[oi->second];
      const reg_t h =a.global[oi2->second];
      if(l == REG_X && h == REG_A || l == REG_A && h == REG_H || l == REG_H)
        return(false);
    }
  
  // In registers.
  if(a.local.find(oi->second) != a.local.end())
    {
      while(++oi != oi_end)
        if(a.local.find(oi->second) == a.local.end())
          return(false);
    }
  else
    {
       while(++oi != oi_end)
        if(a.local.find(oi->second) != a.local.end())
          return(false);
    }
 
  return(true);
}

template <class G_t, class I_t>
static bool inst_sane(const assignment &a, unsigned short int i, const G_t &G, const I_t &I)
{
  const iCode *ic = G[i].ic;

  return(operand_sane(IC_RESULT(ic), a, i, G, I) && operand_sane(IC_LEFT(ic), a, i, G, I) && operand_sane(IC_RIGHT(ic), a, i, G, I));
}

template <class G_t, class I_t>
void assign_operand_for_cost(operand *o, const assignment &a, unsigned short int i, const G_t &G, const I_t &I)
{
  if(!o || !IS_SYMOP(o))
    return;
  symbol *sym = OP_SYMBOL(o);
  operand_map_t::const_iterator oi, oi_end;
  for(boost::tie(oi, oi_end) = G[i].operands.equal_range(OP_SYMBOL_CONST(o)->key); oi != oi_end; ++oi)
    {
      var_t v = oi->second;
      if(a.global[v] >= 0)
        { 
          if(I[v].size == 1)
            {
              sym->regs[I[v].byte] = regshc08 + a.global[v];
              sym->accuse = 0;
              sym->isspilt = false;
              sym->nRegs = I[v].size;
            }
          else
            {
              sym->accuse = (I[v].byte == 0 && a.global[v] == REG_X || I[v].byte == 1 && a.global[v] == REG_H) ? ACCUSE_HX : ACCUSE_XA;
              sym->isspilt = false;
              sym->nRegs = 0;
              sym->regs[I[v].byte] = 0;
            }
        }
      else
        {
          sym->isspilt = true;
          sym->accuse = 0;
          sym->nRegs = I[v].size;
          sym->regs[I[v].byte] = 0;
        }
    }
}

template <class G_t, class I_t>
static void assign_operands_for_cost(const assignment &a, unsigned short int i, const G_t &G, const I_t &I)
{
  const iCode *ic = G[i].ic;
  
  if(ic->op == IFX)
    assign_operand_for_cost(IC_COND(ic), a, i, G, I);
  else if(ic->op == JUMPTABLE)
    assign_operand_for_cost(IC_JTCOND(ic), a, i, G, I);
  else
    {
      assign_operand_for_cost(IC_LEFT(ic), a, i, G, I);
      assign_operand_for_cost(IC_RIGHT(ic), a, i, G, I);
      assign_operand_for_cost(IC_RESULT(ic), a, i, G, I);
    }
    
  if(ic->op == SEND && ic->builtinSEND)
    {
      assign_operands_for_cost(a, *(adjacent_vertices(i, G).first), G, I);
    }
}

// Cost function.
template <class G_t, class I_t>
static float instruction_cost(const assignment &a, unsigned short int i, const G_t &G, const I_t &I)
{
  iCode *ic = G[i].ic;
  float c;

  if(!inst_sane(a, i, G, I))
    return(std::numeric_limits<float>::infinity());

  switch(ic->op)
    {
    //case '!': // SIGSEGV in asmopToBool()
    case '~':
    case UNARYMINUS:
    //case '+': // genPointerGetSetOfs() issue
    case '-':
    case '^':
    case '|':
    case BITWISEAND:
    //case IPUSH: // SIGSEGV in saveRegisters()
    //case IPOP:
    //case CALL:
    //case PCALL:
    //case RETURN:
    case '*':
    case '>':
    case '<':
    case EQ_OP:
    case AND_OP:
    case OR_OP:
    case GETHBIT:
    case LEFT_OP:
    case RIGHT_OP:
    case GET_VALUE_AT_ADDRESS:
    case '=':
    //case IFX: // SIGSEGV in asmopToBool()
    case ADDRESS_OF:
    //case JUMPTABLE:
    //case CAST: // SIGSEGV in asmopToBool()
    //case RECEIVE:
    //case SEND:
    case DUMMY_READ_VOLATILE:
    case CRITICAL:
    case ENDCRITICAL:
      assign_operands_for_cost(a, i, G, I);
      c = dryhc08iCode(ic);
      return(c);
    default:
      return(0.0f);
    }
}

// For early removal of assignments that cannot be extended to valid assignments.
template <class G_t, class I_t>
static bool assignment_hopeless(const assignment &a, unsigned short int i, const G_t &G, const I_t &I, const var_t lastvar)
{
  return(false);
}

template <class G_t, class I_t>
static float rough_cost_estimate(const assignment &a, unsigned short int i, const G_t &G, const I_t &I)
{
  return(0.0f);
}

template <class T_t, class G_t, class I_t>
static bool tree_dec_ralloc(T_t &T, G_t &G, const I_t &I)
{
  bool assignment_optimal;

  con2_t I2(boost::num_vertices(I));
  for(unsigned int i = 0; i < boost::num_vertices(I); i++)
    {
      I2[i].v = I[i].v;
      I2[i].byte = I[i].byte;
      I2[i].size = I[i].size;
      I2[i].name = I[i].name;
    }
  typename boost::graph_traits<I_t>::edge_iterator e, e_end;
  for(boost::tie(e, e_end) = boost::edges(I); e != e_end; ++e)
    add_edge(boost::source(*e, I), boost::target(*e, I), I2);

  assignment ac;
  assignment_optimal = true;
  tree_dec_ralloc_nodes(T, find_root(T), G, I2, ac, &assignment_optimal);

  const assignment &winner = *(T[find_root(T)].assignments.begin());
#ifdef DEBUG_RALLOC_DEC
  std::cout << "Winner: ";
  for(unsigned int i = 0; i < boost::num_vertices(I); i++)
  {
  	std::cout << "(" << i << ", " << int(winner.global[i]) << ") ";
  }
  std::cout << "\n";
  std::cout << "Cost: " << winner.s << "\n";
  std::cout.flush();
#endif
  // Todo: Make this an assertion
  if(winner.global.size() != boost::num_vertices(I))
    {
      std::cerr << "ERROR: No Assignments at root\n";
      exit(-1);
    }

  for(unsigned int v = 0; v < boost::num_vertices(I); v++)
    {
      symbol *sym = (symbol *)(hTabItemWithKey(liveRanges, I[v].v));
      /*if(winner.global[v] >= 0)
        {
          if(winner.global[v] != REG_A && (winner.global[v] != REG_IYL && winner.global[v] != REG_IYH || !OPTRALLOC_IY))
            {
              sym->regs[I[v].byte] = regsZ80 + winner.global[v];
              sym->accuse = 0;
              sym->isspilt = false;
              sym->nRegs = I[v].size;
            }
          else if(winner.global[v] == REG_A)
            {
              sym->accuse = ACCUSE_A;
              sym->isspilt = false;
              sym->nRegs = 0;
              sym->regs[0] = 0;
            }
          else
            {
              sym->accuse = ACCUSE_IY;
              sym->isspilt = false;
              sym->nRegs = 0;
              sym->regs[I[v].byte] = 0;
            }
        }
      else*/
        {
          for(int i = 0; i < I[v].size; i++)
            sym->regs[i] = 0;
          sym->accuse = 0;
          sym->nRegs = I[v].size;
          //spillThis(sym); Leave it to regFix, which can do some spillocation compaction. Todo: Use Thorup instead.
          sym->isspilt = false;
        }
    }

  //for(unsigned int i = 0; i < boost::num_vertices(G); i++)
  //  set_surviving_regs(winner, i, G, I);	// Never freed. Memory leak?

  return(!assignment_optimal);
}

iCode *hc08_ralloc2_cc(ebbIndex *ebbi)
{
  iCode *ic;

#ifdef DEBUG_RALLOC_DEC
  std::cout << "Processing " << currFunc->name << " from " << dstFileName << "\n"; std::cout.flush();
#endif

  cfg_t control_flow_graph;

  con_t conflict_graph;

  ic = create_cfg(control_flow_graph, conflict_graph, ebbi);

  if(options.dump_graphs)
    dump_cfg(control_flow_graph);

  if(options.dump_graphs)
    dump_con(conflict_graph);

  tree_dec_t tree_decomposition;

  thorup_tree_decomposition(tree_decomposition, control_flow_graph);

  nicify(tree_decomposition);

  alive_tree_dec(tree_decomposition, control_flow_graph);

  good_re_root(tree_decomposition);
  nicify(tree_decomposition);
  alive_tree_dec(tree_decomposition, control_flow_graph);

  tree_dec_ralloc(tree_decomposition, control_flow_graph, conflict_graph);

  return(ic);
}

