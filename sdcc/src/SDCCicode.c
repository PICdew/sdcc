/*-------------------------------------------------------------------------

  SDCCicode.c - intermediate code generation etc.
                Written By -  Sandeep Dutta . sandeep.dutta@usa.net (1998)

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

   In other words, you are welcome to use, share and improve this program.
   You are forbidden to forbid anyone else to use, share and improve
   what you give them.   Help stamp out software-hoarding!
-------------------------------------------------------------------------*/

#include "common.h"
#include "newalloc.h"
#include "math.h"
#include "dbuf_string.h"

/*-----------------------------------------------------------------*/
/* global variables       */

set *iCodeChain = NULL;
int iTempNum = 0;
int iTempLblNum = 0;
int operandKey = 0;
int iCodeKey = 0;
char *filename;                 /* current file name */
int lineno = 1;                 /* current line number */
int block;
int scopeLevel;
int seqPoint;

symbol *returnLabel;            /* function return label */
symbol *entryLabel;             /* function entry  label */

/*-----------------------------------------------------------------*/
/* forward definition of some functions */
operand *geniCodeAssign (operand *, operand *, int, int);
static operand *geniCodeArray (operand *, operand *,int);
static operand *geniCodeArray2Ptr (operand *);
operand *geniCodeRValue (operand *, bool);
operand *geniCodeDerefPtr (operand *,int);
int isLvaluereq(int lvl);
static operand *geniCodeCast (sym_link *, operand *, bool);

#define PRINTFUNC(x) void x (struct dbuf_s *dbuf, iCode *ic, char *s)
/* forward definition of ic print functions */
PRINTFUNC (picGetValueAtAddr);
PRINTFUNC (picSetValueAtAddr);
PRINTFUNC (picAddrOf);
PRINTFUNC (picGeneric);
PRINTFUNC (picGenericOne);
PRINTFUNC (picCast);
PRINTFUNC (picAssign);
PRINTFUNC (picLabel);
PRINTFUNC (picGoto);
PRINTFUNC (picIfx);
PRINTFUNC (picJumpTable);
PRINTFUNC (picInline);
PRINTFUNC (picReceive);
PRINTFUNC (picDummyRead);
PRINTFUNC (picCritical);
PRINTFUNC (picEndCritical);

iCodeTable codeTable[] =
{
  {'!', "not", picGenericOne, NULL},
  {'~', "~", picGenericOne, NULL},
  {RRC, "rrc", picGenericOne, NULL},
  {RLC, "rlc", picGenericOne, NULL},
  {GETHBIT, "ghbit", picGenericOne, NULL},
  {GETABIT, "gabit", picGenericOne, NULL},
  {GETBYTE, "gbyte", picGenericOne, NULL},
  {GETWORD, "gword", picGenericOne, NULL},
  {UNARYMINUS, "-", picGenericOne, NULL},
  {IPUSH, "push", picGenericOne, NULL},
  {IPOP, "pop", picGenericOne, NULL},
  {CALL, "call", picGenericOne, NULL},
  {PCALL, "pcall", picGenericOne, NULL},
  {FUNCTION, "proc", picGenericOne, NULL},
  {ENDFUNCTION, "eproc", picGenericOne, NULL},
  {RETURN, "ret", picGenericOne, NULL},
  {'+', "+", picGeneric, NULL},
  {'-', "-", picGeneric, NULL},
  {'*', "*", picGeneric, NULL},
  {'/', "/", picGeneric, NULL},
  {'%', "%", picGeneric, NULL},
  {'>', ">", picGeneric, NULL},
  {'<', "<", picGeneric, NULL},
  {LE_OP, "<=", picGeneric, NULL},
  {GE_OP, ">=", picGeneric, NULL},
  {EQ_OP, "==", picGeneric, NULL},
  {NE_OP, "!=", picGeneric, NULL},
  {AND_OP, "&&", picGeneric, NULL},
  {OR_OP, "||", picGeneric, NULL},
  {'^', "^", picGeneric, NULL},
  {'|', "|", picGeneric, NULL},
  {BITWISEAND, "&", picGeneric, NULL},
  {LEFT_OP, "<<", picGeneric, NULL},
  {RIGHT_OP, ">>", picGeneric, NULL},
  {GET_VALUE_AT_ADDRESS, "@", picGetValueAtAddr, NULL},
  {ADDRESS_OF, "&", picAddrOf, NULL},
  {CAST, "<>", picCast, NULL},
  {'=', ":=", picAssign, NULL},
  {LABEL, "", picLabel, NULL},
  {GOTO, "", picGoto, NULL},
  {JUMPTABLE, "jtab", picJumpTable, NULL},
  {IFX, "if", picIfx, NULL},
  {INLINEASM, "", picInline, NULL},
  {RECEIVE, "recv", picReceive, NULL},
  {SEND, "send", picGenericOne, NULL},
  {ARRAYINIT, "arrayInit", picGenericOne, NULL},
  {DUMMY_READ_VOLATILE, "dummy = (volatile)", picDummyRead, NULL},
  {CRITICAL, "critical_start", picCritical, NULL},
  {ENDCRITICAL, "critical_end", picEndCritical, NULL},
  {SWAP, "swap", picGenericOne, NULL}
};

/*-----------------------------------------------------------------*/
/* operandName - returns the name of the operand                   */
/*-----------------------------------------------------------------*/
int
printOperand (operand * op, FILE * file)
{
  struct dbuf_s dbuf;
  int ret;
  int pnl = 0;

  if (!file)
    {
      file = stdout;
      pnl = 1;
    }
  dbuf_init (&dbuf, 1024);
  ret = dbuf_printOperand(op, &dbuf);
  dbuf_write_and_destroy (&dbuf, file);

  if (pnl)
    putc ('\n', file);

  return ret;
}

int
dbuf_printOperand (operand * op, struct dbuf_s *dbuf)
{
  sym_link *opetype;

  if (!op)
    return 1;

  switch (op->type)
    {

    case VALUE:
      opetype = getSpec (operandType (op));
      if (IS_FLOAT (opetype))
        dbuf_printf (dbuf, "%g {", SPEC_CVAL (opetype).v_float);
      else if (IS_FIXED16X16 (opetype))
        dbuf_printf (dbuf, "%g {", doubleFromFixed16x16(SPEC_CVAL (opetype).v_fixed16x16));
      else
        dbuf_printf (dbuf, "0x%x {", (unsigned int) ulFromVal (op->operand.valOperand));
      dbuf_printTypeChain (operandType (op), dbuf);
      dbuf_append_char (dbuf, '}');
      break;

    case SYMBOL:
#define REGA 1
//#if REGA      /* { */
    if(REGA && !getenv("PRINT_SHORT_OPERANDS")) {
      dbuf_printf (dbuf, "%s [k%d lr%d:%d so:%d]{ ia%d a2p%d re%d rm%d nos%d ru%d dp%d}",           /*{ar%d rm%d ru%d p%d a%d u%d i%d au%d k%d ks%d}"  , */
               (OP_SYMBOL (op)->rname[0] ? OP_SYMBOL (op)->rname : OP_SYMBOL (op)->name),
               op->key,
               OP_LIVEFROM (op), OP_LIVETO (op),
               OP_SYMBOL (op)->stack,
               op->isaddr, op->aggr2ptr, OP_SYMBOL (op)->isreqv,
               OP_SYMBOL (op)->remat,OP_SYMBOL(op)->noSpilLoc,
               OP_SYMBOL(op)->ruonly,OP_SYMBOL(op)->dptr
        );
      {
        dbuf_append_char (dbuf, '{');
        dbuf_printTypeChain (operandType (op), dbuf);
        if (SPIL_LOC (op) && IS_ITEMP (op))
          dbuf_printf (dbuf, "}{ sir@ %s", SPIL_LOC (op)->rname);
        dbuf_append_char (dbuf, '}');
      }

      /* if assigned to registers */
      if (OP_SYMBOL (op)->nRegs)
        {
          if (OP_SYMBOL (op)->isspilt)
            {
              if (!OP_SYMBOL (op)->remat)
                if (OP_SYMBOL (op)->usl.spillLoc)
                  dbuf_printf (dbuf, "[%s]", (OP_SYMBOL (op)->usl.spillLoc->rname[0] ?
                                       OP_SYMBOL (op)->usl.spillLoc->rname :
                                       OP_SYMBOL (op)->usl.spillLoc->name));
                else
                  dbuf_append_str (dbuf, "[err]");
              else
                dbuf_append_str (dbuf, "[remat]");
            }
          else
            {
              int i;
              dbuf_append_char (dbuf, '[');
              for (i = 0; i < OP_SYMBOL (op)->nRegs; i++)
                dbuf_printf (dbuf, "%s ", port->getRegName (OP_SYMBOL (op)->regs[i]));
              dbuf_append_char (dbuf, ']');
            }
        }
//#else         /* } else { */
    } else {
      /* (getenv("PRINT_SHORT_OPERANDS") != NULL) */
      dbuf_printf (dbuf, "%s ", (OP_SYMBOL (op)->rname[0] ? OP_SYMBOL (op)->rname : OP_SYMBOL (op)->name));

      if(getenv("PRINT_SHORT_OPERANDS")[0] < '1')
        {
          dbuf_printf (dbuf, "[lr%d:%d so:%d]",
               OP_LIVEFROM (op), OP_LIVETO (op),
               OP_SYMBOL (op)->stack);
        }

      if(getenv("PRINT_SHORT_OPERANDS")[0] < '2')
        {
          dbuf_append_char (dbuf, '{');
          dbuf_printTypeChain (operandType (op), dbuf);
          if (SPIL_LOC (op) && IS_ITEMP (op))
              dbuf_printf (dbuf, "}{ sir@ %s", SPIL_LOC (op)->rname);
          dbuf_append_char (dbuf, '}');
        }

      /* if assigned to registers */
      if (OP_SYMBOL (op)->nRegs)
        {
          if (OP_SYMBOL (op)->isspilt)
            {
              if (!OP_SYMBOL (op)->remat)
                if (OP_SYMBOL (op)->usl.spillLoc)
                  dbuf_printf (dbuf, "[%s]", (OP_SYMBOL (op)->usl.spillLoc->rname[0] ?
                                       OP_SYMBOL (op)->usl.spillLoc->rname :
                                       OP_SYMBOL (op)->usl.spillLoc->name));
                else
                  dbuf_append_str (dbuf, "[err]");
              else
                dbuf_append_str (dbuf, "[remat]");
            }
          else
            {
              int i;
              dbuf_append_char (dbuf, '[');
              for (i = 0; i < OP_SYMBOL (op)->nRegs; i++)
                dbuf_printf (dbuf, "%s ", port->getRegName (OP_SYMBOL (op)->regs[i]));
              dbuf_append_char (dbuf, ']');
            }
        }
//#endif                /* } */
    }
      break;

    case TYPE:
      dbuf_append_char (dbuf, '(');
      dbuf_printTypeChain (op->operand.typeOperand, dbuf);
      dbuf_append_char (dbuf, ')');
      break;
    }

  return 0;
}


/*-----------------------------------------------------------------*/
/*                    print functions                              */
/*-----------------------------------------------------------------*/
PRINTFUNC (picGetValueAtAddr)
{
  dbuf_append_char (dbuf, '\t');
  dbuf_printOperand (IC_RESULT (ic), dbuf);
  dbuf_append_str (dbuf, " = ");
  dbuf_append_str (dbuf, "@[");
  dbuf_printOperand (IC_LEFT (ic), dbuf);
  dbuf_append_str (dbuf, "]\n");
}

PRINTFUNC (picSetValueAtAddr)
{
  dbuf_append_char (dbuf, '\t');
  dbuf_append_str (dbuf, "*[");
  dbuf_printOperand (IC_LEFT (ic), dbuf);
  dbuf_append_str (dbuf, "] = ");
  dbuf_printOperand (IC_RIGHT (ic), dbuf);
  dbuf_append_char (dbuf, '\n');
}

PRINTFUNC (picAddrOf)
{
  dbuf_append_char (dbuf, '\t');
  dbuf_printOperand (IC_RESULT (ic), dbuf);
  if (IS_ITEMP (IC_LEFT (ic)))
    dbuf_append_str (dbuf, " = ");
  else
    dbuf_append_str (dbuf, " = &[");
  dbuf_printOperand (IC_LEFT (ic), dbuf);
  if (IC_RIGHT (ic))
    {
      if (IS_ITEMP (IC_LEFT (ic)))
        dbuf_append_str (dbuf, " offsetAdd ");
      else
        dbuf_append_str (dbuf, " , ");
      dbuf_printOperand (IC_RIGHT (ic), dbuf);
    }
  if (IS_ITEMP (IC_LEFT (ic)))
    dbuf_append_char (dbuf, '\n');
  else
    dbuf_append_str (dbuf, "]\n");
}

PRINTFUNC (picJumpTable)
{
  symbol *sym;

  dbuf_append_char (dbuf, '\t');
  dbuf_printf (dbuf, "%s\t", s);
  dbuf_printOperand (IC_JTCOND (ic), dbuf);
  for (sym = setFirstItem (IC_JTLABELS (ic)); sym;
       sym = setNextItem (IC_JTLABELS (ic)))
    dbuf_printf (dbuf, "; %s", sym->name);
  dbuf_append_char (dbuf, '\n');
}

PRINTFUNC (picGeneric)
{
  dbuf_append_char (dbuf, '\t');
  dbuf_printOperand (IC_RESULT (ic), dbuf);
  dbuf_append_str (dbuf, " = ");
  dbuf_printOperand (IC_LEFT (ic), dbuf);
  dbuf_printf (dbuf, " %s ", s);
  dbuf_printOperand (IC_RIGHT (ic), dbuf);
  dbuf_append_char (dbuf, '\n');
}

PRINTFUNC (picGenericOne)
{
  dbuf_append_char (dbuf, '\t');
  if (IC_RESULT (ic))
    {
      dbuf_printOperand (IC_RESULT (ic), dbuf);
      dbuf_append_str (dbuf, " = ");
    }

  if (IC_LEFT (ic))
    {
      dbuf_printf (dbuf, "%s ", s);
      dbuf_printOperand (IC_LEFT (ic), dbuf);
    }

  if (!IC_RESULT (ic) && !IC_LEFT (ic))
    dbuf_append_str (dbuf, s);

  if (ic->op == SEND || ic->op == RECEIVE) {
      dbuf_printf (dbuf,"{argreg = %d}",ic->argreg);
  }
  if (ic->op == IPUSH) {
      dbuf_printf (dbuf,"{parmPush = %d}",ic->parmPush);
  }
  dbuf_append_char (dbuf, '\n');
}

PRINTFUNC (picCast)
{
  dbuf_append_char (dbuf, '\t');
  dbuf_printOperand (IC_RESULT (ic), dbuf);
  dbuf_append_str (dbuf, " = ");
  dbuf_printOperand (IC_LEFT (ic), dbuf);
  dbuf_printOperand (IC_RIGHT (ic), dbuf);
  dbuf_append_char (dbuf, '\n');
}


PRINTFUNC (picAssign)
{
  dbuf_append_char (dbuf, '\t');

  if (IC_RESULT (ic)->isaddr && IS_ITEMP (IC_RESULT (ic)))
    dbuf_append_str (dbuf, "*(");

  dbuf_printOperand (IC_RESULT (ic), dbuf);

  if (IC_RESULT (ic)->isaddr && IS_ITEMP (IC_RESULT (ic)))
    dbuf_append_char (dbuf, ')');

  dbuf_printf (dbuf, " %s ", s);
  dbuf_printOperand (IC_RIGHT (ic), dbuf);

  dbuf_append_char (dbuf, '\n');
}

PRINTFUNC (picLabel)
{
  dbuf_printf (dbuf, " %s($%d) :\n", IC_LABEL (ic)->name, IC_LABEL (ic)->key);
}

PRINTFUNC (picGoto)
{
  dbuf_append_char (dbuf, '\t');
  dbuf_printf (dbuf, " goto %s($%d)\n", IC_LABEL (ic)->name, IC_LABEL (ic)->key);
}

PRINTFUNC (picIfx)
{
  dbuf_append_char (dbuf, '\t');
  dbuf_append_str (dbuf, "if ");
  dbuf_printOperand (IC_COND (ic), dbuf);

  if (!IC_TRUE (ic))
    dbuf_printf (dbuf, " == 0 goto %s($%d)\n", IC_FALSE (ic)->name, IC_FALSE (ic)->key);
  else
    {
      dbuf_printf (dbuf, " != 0 goto %s($%d)", IC_TRUE (ic)->name, IC_TRUE (ic)->key);
      if (IC_FALSE (ic))
        dbuf_printf (dbuf, "; zzgoto %s\n", IC_FALSE (ic)->name);
      dbuf_append_char (dbuf, '\n');
    }
}

PRINTFUNC (picInline)
{
  dbuf_append_str (dbuf, IC_INLINE (ic));
}

PRINTFUNC (picReceive)
{
  dbuf_printOperand (IC_RESULT (ic), dbuf);
  dbuf_printf (dbuf, " = %s ", s);
  dbuf_printOperand (IC_LEFT (ic), dbuf);
  dbuf_append_char (dbuf, '\n');
}

PRINTFUNC (picDummyRead)
{
  dbuf_append_char (dbuf, '\t');
  dbuf_printf (dbuf, "%s ", s);
  dbuf_printOperand (IC_RIGHT (ic), dbuf);
  dbuf_append_char (dbuf, '\n');
}

PRINTFUNC (picCritical)
{
  dbuf_append_char (dbuf, '\t');
  if (IC_RESULT (ic))
    dbuf_printOperand (IC_RESULT (ic), dbuf);
  else
    dbuf_append_str (dbuf, "(stack)");
  dbuf_printf (dbuf, " = %s ", s);
  dbuf_append_char (dbuf, '\n');
}

PRINTFUNC (picEndCritical)
{
  dbuf_append_char (dbuf, '\t');
  dbuf_printf (dbuf, "%s = ", s);
  if (IC_RIGHT (ic))
    dbuf_printOperand (IC_RIGHT (ic), dbuf);
  else
    dbuf_append_str (dbuf, "(stack)");
  dbuf_append_char (dbuf, '\n');
}

/*-----------------------------------------------------------------*/
/* piCode - prints one iCode                                       */
/*-----------------------------------------------------------------*/
int
piCode (void *item, FILE * of)
{
  iCode *ic = item;
  iCodeTable *icTab;
  struct dbuf_s dbuf;

  if (!of)
    of = stdout;

  icTab = getTableEntry (ic->op);
  fprintf (of, "%s(%d:%d:%d:%d:%d)\t",
           ic->filename, ic->lineno,
           ic->seq, ic->key, ic->depth, ic->supportRtn);
  dbuf_init (&dbuf, 1024);
  icTab->iCodePrint (&dbuf, ic, icTab->printName);
  dbuf_write_and_destroy (&dbuf, of);
  return 1;
}

void PICC(iCode *ic)
{
        printiCChain(ic,stdout);
}
/*-----------------------------------------------------------------*/
/* printiCChain - prints intermediate code for humans              */
/*-----------------------------------------------------------------*/
void
printiCChain (iCode * icChain, FILE * of)
{
  iCode *loop;
  iCodeTable *icTab;
  struct dbuf_s dbuf;

  if (!of)
    of = stdout;
  for (loop = icChain; loop; loop = loop->next)
    {
      if ((icTab = getTableEntry (loop->op)))
        {
          fprintf (of, "%s(l%d:s%d:k%d:d%d:s%d)\t",
                   loop->filename, loop->lineno,
                   loop->seq, loop->key, loop->depth, loop->supportRtn);

          dbuf_init(&dbuf, 1024);
          icTab->iCodePrint (&dbuf, loop, icTab->printName);
          dbuf_write_and_destroy (&dbuf, of);
          ////
          fflush(of);
        }
    }
}


/*-----------------------------------------------------------------*/
/* newOperand - allocate, init & return a new iCode                */
/*-----------------------------------------------------------------*/
operand *
newOperand ()
{
  operand *op;

  op = Safe_alloc ( sizeof (operand));

  op->key = 0;
  return op;
}

/*-----------------------------------------------------------------*/
/* newiCode - create and return a new iCode entry initialised      */
/*-----------------------------------------------------------------*/
iCode *
newiCode (int op, operand * left, operand * right)
{
  iCode *ic;

  ic = Safe_alloc ( sizeof (iCode));

  ic->seqPoint = seqPoint;
  ic->filename = filename;
  ic->lineno = lineno;
  ic->block = block;
  ic->level = scopeLevel;
  ic->op = op;
  ic->key = iCodeKey++;
  IC_LEFT (ic) = left;
  IC_RIGHT (ic) = right;

  return ic;
}

/*-----------------------------------------------------------------*/
/* newiCode for conditional statements                             */
/*-----------------------------------------------------------------*/
iCode *
newiCodeCondition (operand * condition,
                   symbol * trueLabel,
                   symbol * falseLabel)
{
  iCode *ic;

  if (IS_VOID(operandType(condition))) {
    werror(E_VOID_VALUE_USED);
  }

  ic = newiCode (IFX, NULL, NULL);
  IC_COND (ic) = condition;
  IC_TRUE (ic) = trueLabel;
  IC_FALSE (ic) = falseLabel;
  return ic;
}

/*-----------------------------------------------------------------*/
/* newiCodeLabelGoto - unconditional goto statement| label stmnt   */
/*-----------------------------------------------------------------*/
iCode *
newiCodeLabelGoto (int op, symbol * label)
{
  iCode *ic;

  ic = newiCode (op, NULL, NULL);
  ic->op = op;
  ic->label = label;
  IC_LEFT (ic) = NULL;
  IC_RIGHT (ic) = NULL;
  IC_RESULT (ic) = NULL;
  return ic;
}

/*-----------------------------------------------------------------*/
/* newiTemp - allocate & return a newItemp Variable                */
/*-----------------------------------------------------------------*/
symbol *
newiTemp (char *s)
{
  symbol *itmp;

  if (s)
  {
      SNPRINTF (buffer, sizeof(buffer), "%s", s);
  }
  else
  {
      SNPRINTF (buffer, sizeof(buffer), "iTemp%d", iTempNum++);
  }

  itmp = newSymbol (buffer, 1);
  strncpyz (itmp->rname, itmp->name, SDCC_NAME_MAX);
  itmp->isitmp = 1;

  return itmp;
}

/*-----------------------------------------------------------------*/
/* newiTempLabel - creates a temp variable label                   */
/*-----------------------------------------------------------------*/
symbol *
newiTempLabel (char *s)
{
  symbol *itmplbl;

  /* check if this already exists */
  if (s && (itmplbl = findSym (LabelTab, NULL, s)))
    return itmplbl;

  if (s)
    {
      itmplbl = newSymbol (s, 1);
    }
  else
    {
      SNPRINTF (buffer, sizeof(buffer), "iTempLbl%d", iTempLblNum++);
      itmplbl = newSymbol (buffer, 1);
    }

  itmplbl->isitmp = 1;
  itmplbl->islbl = 1;
  itmplbl->key = labelKey++;
  addSym (LabelTab, itmplbl, itmplbl->name, 0, 0, 0);
  return itmplbl;
}

/*-----------------------------------------------------------------*/
/* newiTempLoopHeaderLabel - creates a new loop header label       */
/*-----------------------------------------------------------------*/
symbol *
newiTempLoopHeaderLabel (bool pre)
{
  symbol *itmplbl;

  SNPRINTF (buffer, sizeof(buffer), pre ? "preHeaderLbl%d" : LOOPEXITLBL "%d",
            iTempLblNum++);
  itmplbl = newSymbol (buffer, 1);

  itmplbl->isitmp = 1;
  itmplbl->islbl = 1;
  itmplbl->key = labelKey++;
  addSym (LabelTab, itmplbl, itmplbl->name, 0, 0, 0);
  return itmplbl;
}


/*-----------------------------------------------------------------*/
/* initiCode - initialises some iCode related stuff                */
/*-----------------------------------------------------------------*/
void
initiCode ()
{

}

/*-----------------------------------------------------------------*/
/* copyiCode - make a copy of the iCode given                      */
/*-----------------------------------------------------------------*/
iCode *
copyiCode (iCode * ic)
{
  iCode *nic = newiCode (ic->op, NULL, NULL);

  nic->filename = ic->filename;
  nic->lineno = ic->lineno;
  nic->block = ic->block;
  nic->level = ic->level;
  nic->parmBytes = ic->parmBytes;

  /* deal with the special cases first */
  switch (ic->op)
    {
    case IFX:
      IC_COND (nic) = operandFromOperand (IC_COND (ic));
      IC_TRUE (nic) = IC_TRUE (ic);
      IC_FALSE (nic) = IC_FALSE (ic);
      break;

    case JUMPTABLE:
      IC_JTCOND (nic) = operandFromOperand (IC_JTCOND (ic));
      IC_JTLABELS (nic) = IC_JTLABELS (ic);
      break;

    case CALL:
    case PCALL:
      IC_RESULT (nic) = operandFromOperand (IC_RESULT (ic));
      IC_LEFT (nic) = operandFromOperand (IC_LEFT (ic));
      break;

    case INLINEASM:
      IC_INLINE (nic) = IC_INLINE (ic);
      break;

    case ARRAYINIT:
      IC_ARRAYILIST(nic) = IC_ARRAYILIST(ic);
      break;

    default:
      IC_RESULT (nic) = operandFromOperand (IC_RESULT (ic));
      IC_LEFT (nic) = operandFromOperand (IC_LEFT (ic));
      IC_RIGHT (nic) = operandFromOperand (IC_RIGHT (ic));
    }

  return nic;
}

/*-----------------------------------------------------------------*/
/* getTableEntry - gets the table entry for the given operator     */
/*-----------------------------------------------------------------*/
iCodeTable *
getTableEntry (int oper)
{
  unsigned i;

  for (i = 0; i < (sizeof (codeTable) / sizeof (iCodeTable)); i++)
    if (oper == codeTable[i].icode)
      return &codeTable[i];

  return NULL;
}

/*-----------------------------------------------------------------*/
/* newiTempOperand - new intermediate temp operand                 */
/*-----------------------------------------------------------------*/
operand *
newiTempOperand (sym_link * type, char throwType)
{
  symbol *itmp;
  operand *op = newOperand ();
  sym_link *etype;

  op->type = SYMBOL;
  itmp = newiTemp (NULL);

  etype = getSpec (type);

  if (IS_LITERAL (etype))
    throwType = 0;

  /* copy the type information */
  if (type)
    itmp->etype = getSpec (itmp->type = (throwType ? type :
                                         copyLinkChain (type)));
  if (IS_LITERAL (itmp->etype))
    {
      SPEC_SCLS (itmp->etype) = S_REGISTER;
      SPEC_OCLS (itmp->etype) = reg;
    }

  op->operand.symOperand = itmp;
  op->key = itmp->key = ++operandKey;
  return op;
}

/*-----------------------------------------------------------------*/
/* operandType - returns the type chain for an operand             */
/*-----------------------------------------------------------------*/
sym_link *
operandType (operand * op)
{
  /* depending on type of operand */
  switch (op->type)
    {
    case VALUE:
      return op->operand.valOperand->type;

    case SYMBOL:
      return op->operand.symOperand->type;

    case TYPE:
      return op->operand.typeOperand;

    default:
      werror (E_INTERNAL_ERROR, __FILE__, __LINE__,
              " operand type not known ");
      assert (0);               /* should never come here */
      /*  Just to keep the compiler happy */
      return (sym_link *) 0;
    }
}

/*-----------------------------------------------------------------*/
/* operandSize - returns size of an operand in bytes               */
/*-----------------------------------------------------------------*/
unsigned int
operandSize (operand * op)
{
  sym_link *type;

  /* if nothing return 0 */
  if (!op)
    return 0;

  type = operandType (op);
  if (op->aggr2ptr == 2)
    type = type->next;
  return getSize (type);
}

/*-----------------------------------------------------------------*/
/* isParamterToCall - will return 1 if op is a parameter to args   */
/*-----------------------------------------------------------------*/
int
isParameterToCall (value * args, operand * op)
{
  value *tval = args;

  wassert (IS_SYMOP(op));

  while (tval)
    {
      if (tval->sym &&
          isSymbolEqual (op->operand.symOperand, tval->sym))
        return 1;
      tval = tval->next;
    }
  return 0;
}

/*-----------------------------------------------------------------*/
/* isOperandGlobal   - return 1 if operand is a global variable    */
/*-----------------------------------------------------------------*/
int
isOperandGlobal (operand * op)
{
  if (!op)
    return 0;

  if (IS_ITEMP (op))
    return 0;

  if (IS_SYMOP(op) &&
      (op->operand.symOperand->level == 0 ||
       IS_STATIC (op->operand.symOperand->etype) ||
       IS_EXTERN (op->operand.symOperand->etype))
    )
    return 1;

  return 0;
}

/*-----------------------------------------------------------------*/
/* isOperandVolatile - return 1 if the operand is volatile         */
/*-----------------------------------------------------------------*/
int
isOperandVolatile (operand * op, bool chkTemp)
{
  if (IS_ITEMP (op) && !chkTemp)
    return 0;

  return IS_VOLATILE (operandType (op));
}

/*-----------------------------------------------------------------*/
/* isOperandLiteral - returns 1 if an operand contains a literal   */
/*-----------------------------------------------------------------*/
int
isOperandLiteral (operand * op)
{
  sym_link *opetype;

  if (!op)
    return 0;

  opetype = getSpec (operandType (op));

  if (IS_LITERAL (opetype))
    return 1;

  return 0;
}

/*-----------------------------------------------------------------*/
/* isOperandInFarSpace - will return true if operand is in farSpace */
/*-----------------------------------------------------------------*/
bool
isOperandInFarSpace (operand * op)
{
  sym_link *etype;

  if (!op)
    return FALSE;

  if (!IS_SYMOP (op))
    return FALSE;

  if (!IS_TRUE_SYMOP (op))
    {
      if (SPIL_LOC (op))
        etype = SPIL_LOC (op)->etype;
      else
        return FALSE;
    }
  else
    {
      etype = getSpec (operandType (op));
    }
  return (IN_FARSPACE (SPEC_OCLS (etype)) ? TRUE : FALSE);
}

/*-----------------------------------------------------------------*/
/* isOperandInPagedSpace - return true if operand is in pagedSpace */
/*-----------------------------------------------------------------*/
bool
isOperandInPagedSpace (operand * op)
{
  sym_link *etype;

  if (!op)
    return FALSE;

  if (!IS_SYMOP (op))
    return FALSE;

  if (!IS_TRUE_SYMOP (op))
    {
      if (SPIL_LOC (op))
        etype = SPIL_LOC (op)->etype;
      else
        return FALSE;
    }
  else
    {
      etype = getSpec (operandType (op));
    }
  return (IN_PAGEDSPACE (SPEC_OCLS (etype)) ? TRUE : FALSE);
}

/*------------------------------------------------------------------*/
/* isOperandInDirSpace - will return true if operand is in dirSpace */
/*------------------------------------------------------------------*/
bool
isOperandInDirSpace (operand * op)
{
  sym_link *etype;

  if (!op)
    return FALSE;

  if (!IS_SYMOP (op))
    return FALSE;

  if (!IS_TRUE_SYMOP (op))
    {
      if (SPIL_LOC (op))
        etype = SPIL_LOC (op)->etype;
      else
        return FALSE;
    }
  else
    {
      etype = getSpec (operandType (op));
    }
  return (IN_DIRSPACE (SPEC_OCLS (etype)) ? TRUE : FALSE);
}

/*--------------------------------------------------------------------*/
/* isOperandInCodeSpace - will return true if operand is in codeSpace */
/*--------------------------------------------------------------------*/
bool
isOperandInCodeSpace (operand * op)
{
  sym_link *etype;

  if (!op)
    return FALSE;

  if (!IS_SYMOP (op))
    return FALSE;

  etype = getSpec (operandType (op));

  if (!IS_TRUE_SYMOP (op))
    {
      if (SPIL_LOC (op))
        etype = SPIL_LOC (op)->etype;
      else
        return FALSE;
    }
  else
    {
      etype = getSpec (operandType (op));
    }
  return (IN_CODESPACE (SPEC_OCLS (etype)) ? TRUE : FALSE);
}

/*-----------------------------------------------------------------*/
/* isOperandOnStack - will return true if operand is on stack      */
/*-----------------------------------------------------------------*/
bool
isOperandOnStack (operand * op)
{
  sym_link *etype;

  if (!op)
    return FALSE;

  if (!IS_SYMOP (op))
    return FALSE;

  etype = getSpec (operandType (op));
  if (IN_STACK (etype) ||
      OP_SYMBOL(op)->onStack ||
      (SPIL_LOC(op) && SPIL_LOC(op)->onStack))
    return TRUE;

  return FALSE;
}

/*-----------------------------------------------------------------*/
/* isOclsExpensive - will return true if accesses to an output     */
/*                   storage class are expensive                   */
/*-----------------------------------------------------------------*/
bool
isOclsExpensive (struct memmap *oclass)
{
  if (port->oclsExpense)
    return port->oclsExpense (oclass) > 0;

  /* In the absence of port specific guidance, assume only */
  /* farspace is expensive. */
  return IN_FARSPACE (oclass);
}

/*-----------------------------------------------------------------*/
/* isiCodeInFunctionCall - return TRUE if an iCode is between a    */
/*   CALL/PCALL and the first IPUSH/SEND associated with the call  */
/*-----------------------------------------------------------------*/
int
isiCodeInFunctionCall (iCode * ic)
{
  iCode * lic = ic;

  /* Find the next CALL/PCALL */
  while (lic)
    {
      if (lic->op == CALL || lic->op == PCALL)
        break;
      lic = lic->next;
    }

  if (!lic)
    return FALSE;

  /* A function call was found. Scan backwards and see if an */
  /* IPUSH or SEND is encountered */
  while (ic)
    {
      if (lic != ic && (ic->op == CALL || ic->op == PCALL))
        return FALSE;
      if (ic->op == SEND || (ic->op == IPUSH && ic->parmPush))
        return TRUE;
      ic = ic->prev;
    }

  return FALSE;
}

/*-----------------------------------------------------------------*/
/* operandLitValue - literal value of an operand                   */
/*-----------------------------------------------------------------*/
double
operandLitValue (operand * op)
{
  assert (isOperandLiteral (op));

  return floatFromVal (op->operand.valOperand);
}

/*-----------------------------------------------------------------*/
/* getBuiltInParms - returns parameters to a builtin function      */
/*-----------------------------------------------------------------*/
iCode *getBuiltinParms (iCode *ic, int *pcount, operand **parms)
{
    sym_link *ftype;

    *pcount = 0;
    /* builtin function uses only SEND for parameters */
    while (ic->op != CALL) {
        assert(ic->op == SEND && ic->builtinSEND);
        ic->generated = 1;    /* mark the icode as generated */
        parms[*pcount] = IC_LEFT(ic);
        ic = ic->next;
        (*pcount)++;
    }

    ic->generated = 1;
    /* make sure this is a builtin function call */
    assert(IS_SYMOP(IC_LEFT(ic)));
    ftype = operandType(IC_LEFT(ic));
    assert(IFFUNC_ISBUILTIN(ftype));
    return ic;
}

/*-----------------------------------------------------------------*/
/* operandOperation - performs operations on operands             */
/*-----------------------------------------------------------------*/
operand *
operandOperation (operand * left, operand * right,
                  int op, sym_link * type)
{
  sym_link *let , *ret=NULL;
  operand *retval = (operand *) 0;

  assert (isOperandLiteral (left));
  let = getSpec(operandType(left));
  if (right) {
    assert (isOperandLiteral (right));
    ret = getSpec(operandType(right));
  }

  switch (op)
    {
    case '+':
      retval = operandFromValue (valCastLiteral (type,
                                                 operandLitValue (left) +
                                                 operandLitValue (right)));
      break;
    case '-':
      retval = operandFromValue (valCastLiteral (type,
                                                 operandLitValue (left) -
                                                 operandLitValue (right)));
      break;
    case '*':
      /*
      retval = operandFromValue (valCastLiteral (type,
                                                 operandLitValue (left) *
                                                 operandLitValue (right)));
      This could be all we've to do, but with gcc we've to take care about
      overflows. Two examples:
      ULONG_MAX * ULONG_MAX doesn't fit into a double, some of the least
      significant bits are lost (52 in fraction, 63 bits would be
      necessary to keep full precision).
      If the resulting double value is greater than ULONG_MAX (resp.
      USHRT_MAX, ...), then 0 will be assigned to v_ulong (resp. u_uint, ...)!
      */

      /* if it is not a specifier then we can assume that */
      /* it will be an unsigned long                      */
      if (IS_INT (type) ||
          !IS_SPEC (type))
        {
          /* long is handled here, because it can overflow with double */
          if (IS_LONG (type) ||
              !IS_SPEC (type))
          /* signed and unsigned mul are the same, as long as the precision
             of the result isn't bigger than the precision of the operands. */
            retval = operandFromValue (valCastLiteral (type,
                     (TYPE_TARGET_ULONG) double2ul (operandLitValue (left)) *
                     (TYPE_TARGET_ULONG) double2ul (operandLitValue (right))));
          else if (IS_UNSIGNED (type)) /* unsigned int */
            {
              /* unsigned int is handled here in order to detect overflow */
              TYPE_TARGET_ULONG ul = (TYPE_TARGET_UINT) double2ul (operandLitValue (left)) *
                                     (TYPE_TARGET_UINT) double2ul (operandLitValue (right));

              retval = operandFromValue (valCastLiteral (type, (TYPE_TARGET_UINT) ul));
              if (ul != (TYPE_TARGET_UINT) ul)
                werror (W_INT_OVL);
            }
          else /* signed int */
            {
              /* signed int is handled here in order to detect overflow */
              TYPE_TARGET_LONG l = (TYPE_TARGET_INT) operandLitValue (left) *
                                   (TYPE_TARGET_INT) operandLitValue (right);

              retval = operandFromValue (valCastLiteral (type, (TYPE_TARGET_INT) l));
              if (l != (TYPE_TARGET_INT) l)
                werror (W_INT_OVL);
            }
        }
      else
        /* all others go here: */
        retval = operandFromValue (valCastLiteral (type,
                                                   operandLitValue (left) *
                                                   operandLitValue (right)));
      break;
    case '/':
      if (IS_UNSIGNED (type))
        {
          if ((TYPE_TARGET_ULONG) double2ul (operandLitValue (right)) == 0)
            {
              werror (E_DIVIDE_BY_ZERO);
              retval = right;
            }
          SPEC_USIGN (let) = 1;
          SPEC_USIGN (ret) = 1;
          retval = operandFromValue (valCastLiteral (type,
                                    (TYPE_TARGET_ULONG) double2ul (operandLitValue (left)) /
                                    (TYPE_TARGET_ULONG) double2ul (operandLitValue (right))));
        }
      else
        {
          if (operandLitValue (right) == 0)
            {
              werror (E_DIVIDE_BY_ZERO);
              retval = right;
            }
          retval = operandFromValue (valCastLiteral (type,
                                                 operandLitValue (left) /
                                                 operandLitValue (right)));
        }
      break;
    case '%':
      if ((TYPE_TARGET_ULONG) double2ul (operandLitValue (right)) == 0)
        {
          werror (E_DIVIDE_BY_ZERO);
          retval = right;
        }
      else
        {
          if (IS_UNSIGNED (type))
            retval = operandFromLit ((TYPE_TARGET_ULONG) double2ul (operandLitValue (left)) %
                                     (TYPE_TARGET_ULONG) double2ul (operandLitValue (right)));
          else
            retval = operandFromLit ((TYPE_TARGET_LONG) operandLitValue (left) %
                                     (TYPE_TARGET_LONG) operandLitValue (right));
        }
      break;
    case LEFT_OP:
      /* The number of left shifts is always unsigned. Signed doesn't make
         sense here. Shifting by a negative number is impossible. */
      retval = operandFromValue (valCastLiteral (type,
                                 ((TYPE_TARGET_ULONG) double2ul (operandLitValue (left)) <<
                                  (TYPE_TARGET_ULONG) double2ul (operandLitValue (right)))));
      break;
    case RIGHT_OP:
      /* The number of right shifts is always unsigned. Signed doesn't make
         sense here. Shifting by a negative number is impossible. */
      if (IS_UNSIGNED(let))
        /* unsigned: logic shift right */
        retval = operandFromLit ((TYPE_TARGET_ULONG) double2ul (operandLitValue (left)) >>
                                 (TYPE_TARGET_ULONG) double2ul (operandLitValue (right)));
      else
        /* signed: arithmetic shift right */
        retval = operandFromLit ((TYPE_TARGET_LONG) operandLitValue (left) >>
                                 (TYPE_TARGET_ULONG) double2ul (operandLitValue (right)));
      break;
    case EQ_OP:
      if (IS_FLOAT (let) || IS_FLOAT (ret))
        {
          retval = operandFromLit (operandLitValue (left) ==
                                   operandLitValue (right));
        }
      else if (IS_FIXED16X16 (let) || IS_FIXED16X16 (ret))
        {
          retval = operandFromLit (operandLitValue (left) ==
                                   operandLitValue (right));
        }
      else
        {
          /* this op doesn't care about signedness */
          TYPE_TARGET_ULONG l, r;

          l = (TYPE_TARGET_ULONG) double2ul (operandLitValue (left));
          r = (TYPE_TARGET_ULONG) double2ul (operandLitValue (right));
          /* In order to correctly compare 'signed int' and 'unsigned int' it's
             neccessary to strip them to 16 bit.
             Literals are reduced to their cheapest type, therefore left and
             right might have different types. It's neccessary to find a
             common type: int (used for char too) or long */
          if (!IS_LONG (let) &&
              !IS_LONG (ret))
            {
              r = (TYPE_TARGET_UINT) r;
              l = (TYPE_TARGET_UINT) l;
            }
          retval = operandFromLit (l == r);
        }
      break;
    case '<':
      retval = operandFromLit (operandLitValue (left) <
                               operandLitValue (right));
      break;
    case LE_OP:
      retval = operandFromLit (operandLitValue (left) <=
                               operandLitValue (right));
      break;
    case NE_OP:
      retval = operandFromLit (operandLitValue (left) !=
                               operandLitValue (right));
      break;
    case '>':
      retval = operandFromLit (operandLitValue (left) >
                               operandLitValue (right));
      break;
    case GE_OP:
      retval = operandFromLit (operandLitValue (left) >=
                               operandLitValue (right));
      break;
    case BITWISEAND:
      retval = operandFromValue (valCastLiteral (type,
                                                 (TYPE_TARGET_ULONG) double2ul (operandLitValue(left)) &
                                                 (TYPE_TARGET_ULONG) double2ul (operandLitValue(right))));
      break;
    case '|':
      retval = operandFromValue (valCastLiteral (type,
                                                 (TYPE_TARGET_ULONG) double2ul (operandLitValue(left)) |
                                                 (TYPE_TARGET_ULONG) double2ul (operandLitValue(right))));
      break;
    case '^':
      retval = operandFromValue (valCastLiteral (type,
                                                 (TYPE_TARGET_ULONG) double2ul (operandLitValue(left)) ^
                                                 (TYPE_TARGET_ULONG) double2ul (operandLitValue(right))));
      break;
    case AND_OP:
      retval = operandFromLit (operandLitValue (left) &&
                               operandLitValue (right));
      break;
    case OR_OP:
      retval = operandFromLit (operandLitValue (left) ||
                               operandLitValue (right));
      break;
    case RRC:
      {
        TYPE_TARGET_ULONG i = (TYPE_TARGET_ULONG) double2ul (operandLitValue (left));

        retval = operandFromLit ((i >> (getSize (operandType (left)) * 8 - 1)) |
                                 (i << 1));
      }
      break;
    case RLC:
      {
        TYPE_TARGET_ULONG i = (TYPE_TARGET_ULONG) double2ul (operandLitValue (left));

        retval = operandFromLit ((i << (getSize (operandType (left)) * 8 - 1)) |
                                 (i >> 1));
      }
      break;
    case GETABIT:
      retval = operandFromLit (((TYPE_TARGET_ULONG) double2ul (operandLitValue(left)) >>
                                (TYPE_TARGET_ULONG) double2ul (operandLitValue(right))) & 1);
      break;
    case GETBYTE:
      retval = operandFromLit (((TYPE_TARGET_ULONG) double2ul (operandLitValue(left)) >>
                                (TYPE_TARGET_ULONG) double2ul (operandLitValue(right)) & 0xFF));
      break;
    case GETWORD:
      retval = operandFromLit (((TYPE_TARGET_ULONG) double2ul (operandLitValue(left)) >>
                                (TYPE_TARGET_ULONG) double2ul (operandLitValue(right)) & 0xFFFF));
      break;

    case GETHBIT:
      retval = operandFromLit (((TYPE_TARGET_ULONG) double2ul (operandLitValue(left)) >>
                                ((getSize (let) * 8) - 1)) & 1);
      break;

    case UNARYMINUS:
      retval = operandFromValue (valCastLiteral (type,
                                                 -1 * operandLitValue (left)));
      break;

    case '~':
      retval = operandFromValue (valCastLiteral (type,
                                                 ~((TYPE_TARGET_ULONG) double2ul (operandLitValue (left)))));
      break;

    case '!':
      retval = operandFromLit (!operandLitValue (left));
      break;

    default:
      werror (E_INTERNAL_ERROR, __FILE__, __LINE__,
              " operandOperation invalid operator ");
      assert (0);
    }

  return retval;
}


/*-----------------------------------------------------------------*/
/* isOperandEqual - compares two operand & return 1 if they r =    */
/*-----------------------------------------------------------------*/
int
isOperandEqual (operand * left, operand * right)
{
  /* if the pointers are equal then they are equal */
  if (left == right)
    return 1;

  /* if either of them null then false */
  if (!left || !right)
    return 0;

  if (left->type != right->type)
    return 0;

  if (IS_SYMOP (left) && IS_SYMOP (right))
    return left->key == right->key;

  /* if types are the same */
  switch (left->type)
    {
    case SYMBOL:
      return isSymbolEqual (left->operand.symOperand,
                            right->operand.symOperand);
    case VALUE:
      return (compareType (left->operand.valOperand->type,
                           right->operand.valOperand->type) &&
              (floatFromVal (left->operand.valOperand) ==
               floatFromVal (right->operand.valOperand)));
    case TYPE:
      if (compareType (left->operand.typeOperand,
                     right->operand.typeOperand) == 1)
        return 1;
    }

  return 0;
}

/*-------------------------------------------------------------------*/
/* isiCodeEqual - compares two iCodes are equal, returns true if yes */
/*-------------------------------------------------------------------*/
int
isiCodeEqual (iCode * left, iCode * right)
{
  /* if the same pointer */
  if (left == right)
    return 1;

  /* if either of them null */
  if (!left || !right)
    return 0;

  /* if operand are the same */
  if (left->op == right->op)
    {

      /* compare all the elements depending on type */
      if (left->op != IFX)
        {
          if (!isOperandEqual (IC_LEFT (left), IC_LEFT (right)))
            return 0;
          if (!isOperandEqual (IC_RIGHT (left), IC_RIGHT (right)))
            return 0;

        }
      else
        {
          if (!isOperandEqual (IC_COND (left), IC_COND (right)))
            return 0;
          if (!isSymbolEqual (IC_TRUE (left), IC_TRUE (right)))
            return 0;
          if (!isSymbolEqual (IC_FALSE (left), IC_FALSE (right)))
            return 0;
        }

      return 1;
    }
  return 0;
}

/*-----------------------------------------------------------------*/
/* newiTempFromOp - create a temp Operand with same attributes     */
/*-----------------------------------------------------------------*/
operand *
newiTempFromOp (operand * op)
{
  operand *nop;

  if (!op)
    return NULL;

  if (!IS_ITEMP (op))
    return op;

  nop = newiTempOperand (operandType (op), TRUE);
  nop->isaddr = op->isaddr;
  nop->isvolatile = op->isvolatile;
  nop->isGlobal = op->isGlobal;
  nop->isLiteral = op->isLiteral;
  nop->usesDefs = op->usesDefs;
  nop->isParm = op->isParm;
  return nop;
}

/*-----------------------------------------------------------------*/
/* operand from operand - creates an operand holder for the type   */
/*-----------------------------------------------------------------*/
operand *
operandFromOperand (operand * op)
{
  operand *nop;

  if (!op)
    return NULL;
  nop = newOperand ();
  nop->type = op->type;
  nop->isaddr = op->isaddr;
  nop->key = op->key;
  nop->isvolatile = op->isvolatile;
  nop->isGlobal = op->isGlobal;
  nop->isLiteral = op->isLiteral;
  nop->usesDefs = op->usesDefs;
  nop->isParm = op->isParm;

  switch (nop->type)
    {
    case SYMBOL:
      nop->operand.symOperand = op->operand.symOperand;
      break;
    case VALUE:
      nop->operand.valOperand = op->operand.valOperand;
      break;
    case TYPE:
      nop->operand.typeOperand = op->operand.typeOperand;
      break;
    }

  return nop;
}

/*-----------------------------------------------------------------*/
/* opFromOpWithDU - makes a copy of the operand and DU chains      */
/*-----------------------------------------------------------------*/
operand *
opFromOpWithDU (operand * op, bitVect * defs, bitVect * uses)
{
  operand *nop = operandFromOperand (op);

  if (nop->type == SYMBOL)
    {
      OP_DEFS (nop) = bitVectCopy (defs);
      OP_USES (nop) = bitVectCopy (uses);
    }

  return nop;
}

/*-----------------------------------------------------------------*/
/* operandFromSymbol - creates an operand from a symbol            */
/*-----------------------------------------------------------------*/
operand *
operandFromSymbol (symbol * sym)
{
  operand *op;
  iCode *ic;
  int ok = 1;
  /* if the symbol's type is a literal */
  /* then it is an enumerator type     */
  if (IS_LITERAL (sym->etype) && SPEC_ENUM (sym->etype))
    return operandFromValue (valFromType (sym->etype));

  if (!sym->key)
    sym->key = ++operandKey;

  /* if this an implicit variable, means struct/union */
  /* member so just return it                         */
  if (sym->implicit || IS_FUNC (sym->type))
    {
      op = newOperand ();
      op->type = SYMBOL;
      op->operand.symOperand = sym;
      op->key = sym->key;
      op->isvolatile = isOperandVolatile (op, TRUE);
      op->isGlobal = isOperandGlobal (op);
      return op;
    }

  /* under the following conditions create a
     register equivalent for a local symbol */
  if (sym->level && sym->etype && SPEC_OCLS (sym->etype) &&
      (IN_FARSPACE (SPEC_OCLS (sym->etype)) &&
      !TARGET_IS_HC08 &&
      (!(options.model == MODEL_FLAT24)) ) &&
      options.stackAuto == 0)
    ok = 0;

  if (!IS_AGGREGATE (sym->type) &&      /* not an aggregate */
      !IS_FUNC (sym->type) &&           /* not a function   */
      !sym->_isparm &&                  /* not a parameter  */
      IS_AUTO (sym) &&                  /* is a local auto variable */
      !sym->addrtaken &&                /* whose address has not been taken */
      !sym->reqv &&                     /* does not already have a reg equivalence */
      !IS_VOLATILE (sym->etype) &&      /* not declared as volatile */
      !sym->islbl &&                    /* not a label */
      ok                                /* farspace check */
    )
    {

      /* we will use it after all optimizations
         and before liveRange calculation */
      sym->reqv = newiTempOperand (sym->type, 0);
      sym->reqv->key = sym->key;
      OP_SYMBOL (sym->reqv)->prereqv = sym;
      OP_SYMBOL (sym->reqv)->key = sym->key;
      OP_SYMBOL (sym->reqv)->isreqv = 1;
      OP_SYMBOL (sym->reqv)->islocal = 1;
      OP_SYMBOL (sym->reqv)->onStack = sym->onStack;
      SPIL_LOC (sym->reqv) = sym;
    }

  if (!IS_AGGREGATE (sym->type))
    {
      op = newOperand ();
      op->type = SYMBOL;
      op->operand.symOperand = sym;
      op->isaddr = 1;
      op->key = sym->key;
      op->isvolatile = isOperandVolatile (op, TRUE);
      op->isGlobal = isOperandGlobal (op);
      op->isPtr = IS_PTR (operandType (op));
      op->isParm = sym->_isparm;
      return op;
    }

  /* create :-                     */
  /*    itemp = &[_symbol]         */

  ic = newiCode (ADDRESS_OF, newOperand (), NULL);
  IC_LEFT (ic)->type = SYMBOL;
  IC_LEFT (ic)->operand.symOperand = sym;
  IC_LEFT (ic)->key = sym->key;
  (IC_LEFT (ic))->isvolatile = isOperandVolatile (IC_LEFT (ic), TRUE);
  (IC_LEFT (ic))->isGlobal = isOperandGlobal (IC_LEFT (ic));
  IC_LEFT (ic)->isPtr = IS_PTR (operandType (IC_LEFT (ic)));

  /* create result */
  IC_RESULT (ic) = newiTempOperand (sym->type, 0);
  if (IS_ARRAY (sym->type))
    {
      IC_RESULT (ic) = geniCodeArray2Ptr (IC_RESULT (ic));
    }
  else
    IC_RESULT (ic)->isaddr = (!IS_AGGREGATE (sym->type));

  ADDTOCHAIN (ic);

  return IC_RESULT (ic);
}

/*-----------------------------------------------------------------*/
/* operandFromValue - creates an operand from value                */
/*-----------------------------------------------------------------*/
operand *
operandFromValue (value * val)
{
  operand *op;

  /* if this is a symbol then do the symbol thing */
  if (val->sym)
    return operandFromSymbol (val->sym);

  /* this is not a symbol */
  op = newOperand ();
  op->type = VALUE;
  op->operand.valOperand = val;
  op->isLiteral = isOperandLiteral (op);
  return op;
}

/*-----------------------------------------------------------------*/
/* operandFromLink - operand from typeChain                        */
/*-----------------------------------------------------------------*/
operand *
operandFromLink (sym_link * type)
{
  operand *op;

  /* operand from sym_link */
  if (!type)
    return NULL;

  op = newOperand ();
  op->type = TYPE;
  op->operand.typeOperand = copyLinkChain (type);
  return op;
}

/*-----------------------------------------------------------------*/
/* operandFromLit - makes an operand from a literal value          */
/*-----------------------------------------------------------------*/
operand *
operandFromLit (double i)
{
  return operandFromValue (valueFromLit (i));
}

/*-----------------------------------------------------------------*/
/* operandFromAst - creates an operand from an ast                 */
/*-----------------------------------------------------------------*/
operand *
operandFromAst (ast * tree, int lvl)
{

  if (!tree)
    return NULL;

  /* depending on type do */
  switch (tree->type)
    {
    case EX_OP:
      return ast2iCode (tree,lvl+1);
      break;

    case EX_VALUE:
      return operandFromValue (tree->opval.val);
      break;

    case EX_LINK:
      return operandFromLink (tree->opval.lnk);
      break;

    default:
      assert (0);
    }

  /*  Just to keep the compiler happy */
  return (operand *) 0;
}

/*-----------------------------------------------------------------*/
/* setOperandType - sets the operand's type to the given type      */
/*-----------------------------------------------------------------*/
void
setOperandType (operand * op, sym_link * type)
{
  /* depending on the type of operand */
  switch (op->type)
    {
    case VALUE:
      op->operand.valOperand->etype =
        getSpec (op->operand.valOperand->type =
                 copyLinkChain (type));
      return;

    case SYMBOL:
      if (op->operand.symOperand->isitmp)
        op->operand.symOperand->etype =
          getSpec (op->operand.symOperand->type =
                   copyLinkChain (type));
      else
        werror (E_INTERNAL_ERROR, __FILE__, __LINE__,
                "attempt to modify type of source");
      return;

    case TYPE:
      op->operand.typeOperand = copyLinkChain (type);
      return;
    }
}

/*-----------------------------------------------------------------*/
/* Get size in byte of ptr need to access an array                 */
/*-----------------------------------------------------------------*/
static unsigned int
getArraySizePtr (operand * op)
{
  sym_link *ltype = operandType(op);

  if(IS_PTR(ltype))
    {
      int size = getSize(ltype);
      return((IS_GENPTR(ltype) && GPTRSIZE > FPTRSIZE) ? (size-1) : size);
    }

  if(IS_ARRAY(ltype))
    {
      sym_link *letype = getSpec(ltype);
      switch (PTR_TYPE (SPEC_OCLS (letype)))
        {
        case IPOINTER:
        case PPOINTER:
        case POINTER:
          return (PTRSIZE);
        case EEPPOINTER:
        case FPOINTER:
        case CPOINTER:
        case FUNCTION:
          return (FPTRSIZE);
        case GPOINTER:
          if (GPTRSIZE > FPTRSIZE)
            return (GPTRSIZE-1);
          else
            return (FPTRSIZE);

        default:
          return (FPTRSIZE);
        }
    }
  return (FPTRSIZE);
}

/*-----------------------------------------------------------------*/
/* perform "usual unary conversions"                               */
/*-----------------------------------------------------------------*/
#if 0
static operand *
usualUnaryConversions (operand * op)
{
  if (IS_INTEGRAL (operandType (op)))
    {
      if (getSize (operandType (op)) < (unsigned int) INTSIZE)
        {
          /* Widen to int. */
          return geniCodeCast (INTTYPE, op, TRUE);
        }
    }
  return op;
}
#endif

/*-----------------------------------------------------------------*/
/* perform "usual binary conversions"                              */
/*-----------------------------------------------------------------*/

static sym_link *
usualBinaryConversions (operand ** op1, operand ** op2,
                        RESULT_TYPE resultType, int op)
{
  sym_link *ctype;
  sym_link *rtype = operandType (*op2);
  sym_link *ltype = operandType (*op1);

  ctype = computeType (ltype, rtype, resultType, op);

  switch (op)
    {
      case '*':
      case '/':
      case '%':
        if (IS_CHAR (getSpec (ltype)) && IS_CHAR (getSpec (rtype)))
          {
            /* one byte operations: keep signedness for code generator */
            return ctype;
          }
        break;
      default:
        break;
    }

  *op1 = geniCodeCast (ctype, *op1, TRUE);
  *op2 = geniCodeCast (ctype, *op2, TRUE);

  return ctype;
}

/*-----------------------------------------------------------------*/
/* geniCodeValueAtAddress - generate intermeditate code for value  */
/*                          at address                             */
/*-----------------------------------------------------------------*/
operand *
geniCodeRValue (operand * op, bool force)
{
  iCode *ic;
  sym_link *type = operandType (op);
  sym_link *etype = getSpec (type);

  /* if this is an array & already */
  /* an address then return this   */
  if (IS_AGGREGATE (type) ||
      (IS_PTR (type) && !force && !op->isaddr))
    return operandFromOperand (op);

  /* if this is not an address then must be */
  /* rvalue already so return this one      */
  if (!op->isaddr)
    return op;

  /* if this is not a temp symbol then */
  if (!IS_ITEMP (op) &&
      !force &&
      !(IN_FARSPACE (SPEC_OCLS (etype)) && !TARGET_IS_HC08))
    {
      op = operandFromOperand (op);
      op->isaddr = 0;
      return op;
    }

  if (IS_SPEC (type) &&
      IS_TRUE_SYMOP (op) &&
      (!(IN_FARSPACE (SPEC_OCLS (etype)) && !TARGET_IS_HC08) ||
      (options.model == MODEL_FLAT24) ))
    {
      op = operandFromOperand (op);
      op->isaddr = 0;
      return op;
    }

  ic = newiCode (GET_VALUE_AT_ADDRESS, op, NULL);
  if (IS_PTR (type) && op->isaddr && force)
    type = type->next;

  type = copyLinkChain (type);

  IC_RESULT (ic) = newiTempOperand (type, 1);
  IC_RESULT (ic)->isaddr = 0;

/*     ic->supportRtn = ((IS_GENPTR(type) | op->isGptr) & op->isaddr); */

  ADDTOCHAIN (ic);

  return IC_RESULT (ic);
}

static DECLARATOR_TYPE
getPtrType(sym_link *type)
{
  //for Z80 anything goes
  if (TARGET_Z80_LIKE)
    return POINTER;

  //preserve original behaviour for PIC16
  if (TARGET_IS_PIC16)
    return POINTER;

  //for HC08 only zeropage ptr is different
  if (TARGET_IS_HC08)
    {
      if (IS_DATA_PTR (type))
        return POINTER;
      else
        return FPOINTER;
    }

  if (IS_DATA_PTR (type) && TARGET_MCS51_LIKE)
    return IPOINTER;
  if (IS_PTR (type))
    return DCL_TYPE (type);
  else if (IS_FUNC (type))
    return CPOINTER;
  else if (IS_ARRAY (type))
    return PTR_TYPE (SPEC_OCLS (getSpec (type)));
  return UPOINTER;
}

/*-----------------------------------------------------------------*/
/* checkPtrQualifiers - check for lost pointer qualifers           */
/*-----------------------------------------------------------------*/
static void
checkPtrQualifiers (sym_link *ltype, sym_link *rtype)
{
  if (IS_PTR(ltype) && IS_PTR(rtype) && !IS_FUNCPTR (ltype))
    {
      if (!IS_CONSTANT(ltype->next) && IS_CONSTANT(rtype->next))
        werror(W_TARGET_LOST_QUALIFIER, "const");
#if 0
      // disabled because SDCC will make all union fields volatile
      // but your ptr to it need not be
      if (!IS_VOLATILE(ltype->next) && IS_VOLATILE(rtype->next))
        werror(W_TARGET_LOST_QUALIFIER, "volatile");
#endif
      if (!IS_RESTRICT(ltype->next) && IS_RESTRICT(rtype->next))
        werror(W_TARGET_LOST_QUALIFIER, "restrict");
    }
}

/*-----------------------------------------------------------------*/
/* geniCodeCast - changes the value from one type to another       */
/*-----------------------------------------------------------------*/
static operand *
geniCodeCast (sym_link * type, operand * op, bool implicit)
{
  iCode *ic;
  sym_link *optype;
  sym_link *opetype = getSpec (optype = operandType (op));
  sym_link *restype;
  int errors=0;

  /* one of them has size zero then error */
  if (IS_VOID (optype))
    {
      werror (E_CAST_ZERO);
      return op;
    }

  if (IS_ITEMP (op) && IS_ARRAY (OP_SYMBOL (op)->type))
    {
      geniCodeArray2Ptr (op);
    }

  /* if the operand is already the desired type then do nothing */
  if (compareType (type, optype) == 1)
    return op;

  /* if this is a literal then just change the type & return */
  if (IS_LITERAL (opetype) && op->type == VALUE && !IS_PTR (type) && !IS_PTR (optype))
    {
      return operandFromValue (valCastLiteral (type, operandLitValue (op)));
    }

  /* if casting to/from pointers, do some checking */
  if (IS_PTR(type)) { // to a pointer
    if (!IS_PTR(optype) && !IS_FUNC(optype) && !IS_AGGREGATE(optype)) { // from a non pointer
      if (IS_INTEGRAL(optype)) {
        // maybe this is NULL, than it's ok.
        if (!(IS_LITERAL(optype) && (SPEC_CVAL(optype).v_ulong ==0))) {
          if (port->s.gptr_size > port->s.fptr_size && IS_GENPTR(type)) {
            // no way to set the storage
            if (IS_LITERAL(optype)) {
              werror(E_LITERAL_GENERIC);
              errors++;
            } else {
              werror(E_NONPTR2_GENPTR);
              errors++;
            }
          } else if (implicit) {
            werror(W_INTEGRAL2PTR_NOCAST);
            errors++;
          }
        }
      } else {
        // shouldn't do that with float, array or structure unless to void
        if (!IS_VOID(getSpec(type)) &&
            !(IS_CODEPTR(type) && IS_FUNC(type->next) && IS_FUNC(optype))) {
          werror(E_INCOMPAT_TYPES);
          errors++;
        }
      }
    } else { // from a pointer to a pointer
      if (IS_GENPTR(type) && IS_VOID(type->next))
        { // cast to void* is always allowed
        }
      else if (IS_GENPTR(optype) && IS_VOID(optype->next))
        { // cast from void* is always allowed
        }
      else if (port->s.gptr_size > port->s.fptr_size /*!TARGET_IS_Z80 && !TARGET_IS_GBZ80*/) {
        // if not a pointer to a function
        if (!(IS_CODEPTR(type) && IS_FUNC(type->next) && IS_FUNC(optype))) {
          if (implicit) { // if not to generic, they have to match
            if (!IS_GENPTR(type) &&
                !((DCL_TYPE(optype) == DCL_TYPE(type)) ||
                  ((DCL_TYPE(optype) == POINTER) && (DCL_TYPE(type) == IPOINTER))
                 )
               )
            {
              werror(E_INCOMPAT_PTYPES);
              errors++;
            }
          }
        }
      }
    }
  } else { // to a non pointer
    if (IS_PTR(optype)) { // from a pointer
      if (implicit) { // sneaky
        if (IS_INTEGRAL(type)) {
          werror(W_PTR2INTEGRAL_NOCAST);
          errors++;
        } else { // shouldn't do that with float, array or structure
          werror(E_INCOMPAT_TYPES);
          errors++;
        }
      }
    }
  }
  if (errors) {
    printFromToType (optype, type);
  }

  /* if they are the same size create an assignment */

  /* This seems very dangerous to me, since there are several */
  /* optimizations (for example, gcse) that don't notice the  */
  /* cast hidden in this assignment and may simplify an       */
  /* iCode to use the original (uncasted) operand.            */
  /* Unfortunately, other things break when this cast is      */
  /* made explicit. Need to fix this someday.                 */
  /* -- EEP, 2004/01/21                                       */
  if (getSize (type) == getSize (optype) &&
      !IS_BOOL (type) &&
      !IS_BITFIELD (type) &&
      !IS_FLOAT (type) &&
      !IS_FLOAT (optype) &&
      !IS_FIXED (type) &&
      !IS_FIXED (optype) &&
      ((IS_SPEC (type) && IS_SPEC (optype)) ||
       (IS_DECL (type) && IS_DECL (optype) && getPtrType (type) == getPtrType (optype))))
    {
      ic = newiCode ('=', NULL, op);
      IC_RESULT (ic) = newiTempOperand (type, 0);
      if (IS_TRUE_SYMOP (op) && !IS_VOLATILE (optype))
          SPIL_LOC (IC_RESULT (ic)) = OP_SYMBOL (op);
      IC_RESULT (ic)->isaddr = 0;
    }
  else
    {
      ic = newiCode (CAST, operandFromLink (type), geniCodeRValue (op, FALSE));
      IC_RESULT (ic) = newiTempOperand (type, 0);
    }

  /* preserve the storage class & output class */
  /* of the original variable                  */
  restype = getSpec (operandType (IC_RESULT (ic)));
  if (!IS_LITERAL(opetype) &&
      !IS_BIT(opetype))
    {
      SPEC_SCLS (restype) = SPEC_SCLS (opetype);
      SPEC_OCLS (restype) = SPEC_OCLS (opetype);
    }
  ADDTOCHAIN (ic);
  return IC_RESULT (ic);
}

/*-----------------------------------------------------------------*/
/* geniCodeLabel - will create a Label                             */
/*-----------------------------------------------------------------*/
void
geniCodeLabel (symbol * label)
{
  iCode *ic;

  ic = newiCodeLabelGoto (LABEL, label);
  ADDTOCHAIN (ic);
}

/*-----------------------------------------------------------------*/
/* geniCodeGoto  - will create a Goto                              */
/*-----------------------------------------------------------------*/
void
geniCodeGoto (symbol * label)
{
  iCode *ic;

  ic = newiCodeLabelGoto (GOTO, label);
  ADDTOCHAIN (ic);
}

/*-----------------------------------------------------------------*/
/* geniCodeMultiply - gen intermediate code for multiplication     */
/*-----------------------------------------------------------------*/
static operand *
geniCodeMultiply (operand * left, operand * right, RESULT_TYPE resultType)
{
  iCode *ic;
  int p2 = 0;
  sym_link *resType;
  LRTYPE;

  /* if they are both literal then we know the result */
  if (IS_LITERAL (letype) && IS_LITERAL (retype))
    return operandFromValue (valMult (left->operand.valOperand,
                                      right->operand.valOperand));

  if (IS_LITERAL(retype)) {
    p2 = powof2 ((TYPE_TARGET_ULONG) ulFromVal (right->operand.valOperand));
  }

  resType = usualBinaryConversions (&left, &right, resultType, '*');
  rtype = operandType (right);
  retype = getSpec (rtype);
  ltype = operandType (left);
  letype = getSpec (ltype);

  /* if the right is a literal & power of 2 */
  /* then make it a left shift              */
  /* code generated for 1 byte * 1 byte literal = 2 bytes result is more
     efficient in most cases than 2 bytes result = 2 bytes << literal
     if port has 1 byte muldiv */
  if ((p2 > 0) && !IS_FLOAT (letype) && !IS_FIXED (letype)
      && !((resultType == RESULT_TYPE_INT) && (getSize (resType) != getSize (ltype))
           && (port->support.muldiv == 1))
      && strcmp (port->target, "pic16") != 0  /* don't shift for pic */
      && strcmp (port->target, "pic14") != 0)
    {
      if ((resultType == RESULT_TYPE_INT) && (getSize (resType) != getSize (ltype)))
        {
          /* LEFT_OP need same size for left and result, */
          left = geniCodeCast (resType, left, TRUE);
          ltype = operandType (left);
        }
      ic = newiCode (LEFT_OP, left, operandFromLit (p2)); /* left shift */
    }
  else
    {
      /* if the size left or right > 1 then support routine */
      if (getSize (ltype) > 1 || getSize (rtype) > 1)
        {
          if (IS_LITERAL (retype))
            ic = newiCode ('*', right, left); /* multiplication by support routine with one literal */
          else
            ic = newiCode ('*', left, right); /* multiplication by support routine */
          ic->supportRtn = 1;
        }
      else
        {
          ic = newiCode ('*', left, right);   /* normal multiplication */
        }
    }
  IC_RESULT (ic) = newiTempOperand (resType, 1);

  ADDTOCHAIN (ic);
  return IC_RESULT (ic);
}

/*-----------------------------------------------------------------*/
/* geniCodeDivision - gen intermediate code for division           */
/*-----------------------------------------------------------------*/
static operand *
geniCodeDivision (operand * left, operand * right, RESULT_TYPE resultType)
{
  iCode *ic;
  int p2 = 0;
  sym_link *resType;
  sym_link *rtype = operandType (right);
  sym_link *retype = getSpec (rtype);
  sym_link *ltype = operandType (left);
  sym_link *letype = getSpec (ltype);

  resType = usualBinaryConversions (&left, &right, resultType, '/');

  /* if the right is a literal & power of 2
     and left is unsigned then make it a
     right shift */
  if (IS_LITERAL (retype) &&
      !IS_FLOAT (letype) &&
      !IS_FIXED (letype) &&
      IS_UNSIGNED(letype) &&
      ((p2 = powof2 ((TYPE_TARGET_ULONG) ulFromVal (right->operand.valOperand))) > 0)) {
    ic = newiCode (RIGHT_OP, left, operandFromLit (p2)); /* right shift */
  }
  else
    {
      ic = newiCode ('/', left, right);         /* normal division */
      /* if the size left or right > 1 then support routine */
      if (getSize (ltype) > 1 || getSize (rtype) > 1)
        ic->supportRtn = 1;
    }
  IC_RESULT (ic) = newiTempOperand (resType, 0);

  ADDTOCHAIN (ic);
  return IC_RESULT (ic);
}
/*-----------------------------------------------------------------*/
/* geniCodeModulus  - gen intermediate code for modulus            */
/*-----------------------------------------------------------------*/
static operand *
geniCodeModulus (operand * left, operand * right, RESULT_TYPE resultType)
{
  iCode *ic;
  sym_link *resType;
  LRTYPE;

  /* if they are both literal then we know the result */
  if (IS_LITERAL (letype) && IS_LITERAL (retype))
    return operandFromValue (valMod (left->operand.valOperand,
                                     right->operand.valOperand));

  resType = usualBinaryConversions (&left, &right, resultType, '%');

  /* now they are the same size */
  ic = newiCode ('%', left, right);

  /* if the size left or right > 1 then support routine */
  if (getSize (ltype) > 1 || getSize (rtype) > 1)
    ic->supportRtn = 1;
  IC_RESULT (ic) = newiTempOperand (resType, 0);

  ADDTOCHAIN (ic);
  return IC_RESULT (ic);
}

/*-----------------------------------------------------------------*/
/* geniCodePtrPtrSubtract - subtracts pointer from pointer         */
/*-----------------------------------------------------------------*/
operand *
geniCodePtrPtrSubtract (operand * left, operand * right)
{
  iCode *ic;
  operand *result;
  LRTYPE;

  /* if they are both literals then */
  if (IS_LITERAL (letype) && IS_LITERAL (retype))
    {
      result = operandFromValue (valMinus (left->operand.valOperand,
                                           right->operand.valOperand));
      goto subtractExit;
    }

  ic = newiCode ('-', left, right);

  IC_RESULT (ic) = result = newiTempOperand (newIntLink (), 1);
  ADDTOCHAIN (ic);

subtractExit:
  if (IS_VOID(ltype->next) || IS_VOID(rtype->next)) {
    return result;
  }

  // should we really do this? is this ANSI?
  return geniCodeDivision (result,
                           operandFromLit (getSize (ltype->next)),
                           FALSE);
}

/*-----------------------------------------------------------------*/
/* geniCodeSubtract - generates code for subtraction               */
/*-----------------------------------------------------------------*/
static operand *
geniCodeSubtract (operand * left, operand * right, RESULT_TYPE resultType)
{
  iCode *ic;
  int isarray = 0;
  sym_link *resType;
  LRTYPE;

  /* if they both pointers then */
  if ((IS_PTR (ltype) || IS_ARRAY (ltype)) &&
      (IS_PTR (rtype) || IS_ARRAY (rtype)))
    return geniCodePtrPtrSubtract (left, right);

  /* if they are both literal then we know the result */
  if (IS_LITERAL (letype) && IS_LITERAL (retype)
      && left->isLiteral && right->isLiteral)
    return operandFromValue (valMinus (left->operand.valOperand,
                                       right->operand.valOperand));

  /* if left is an array or pointer */
  if (IS_PTR (ltype) || IS_ARRAY (ltype))
    {
      isarray = left->isaddr;
      right = geniCodeMultiply (right,
                                operandFromLit (getSize (ltype->next)),
                                (getArraySizePtr(left) >= INTSIZE) ?
                                  RESULT_TYPE_INT :
                                  RESULT_TYPE_CHAR);
      resType = copyLinkChain (IS_ARRAY (ltype) ? ltype->next : ltype);
    }
  else
    {                           /* make them the same size */
      resType = usualBinaryConversions (&left, &right, resultType, '-');
    }

  ic = newiCode ('-', left, right);

  IC_RESULT (ic) = newiTempOperand (resType, 1);
  IC_RESULT (ic)->isaddr = (isarray ? 1 : 0);

  /* if left or right is a float */
  if (IS_FLOAT (ltype) || IS_FLOAT (rtype)
      || IS_FIXED (ltype) || IS_FIXED (rtype))
    ic->supportRtn = 1;

  ADDTOCHAIN (ic);
  return IC_RESULT (ic);
}

/*-----------------------------------------------------------------*/
/* geniCodeAdd - generates iCode for addition                      */
/*-----------------------------------------------------------------*/
static operand *
geniCodeAdd (operand * left, operand * right, RESULT_TYPE resultType, int lvl)
{
  iCode *ic;
  sym_link *resType;
  operand *size;
  int isarray = 0;
  bool indexUnsigned;
  LRTYPE;

  /* if the right side is LITERAL zero */
  /* return the left side              */
  if (IS_LITERAL (retype) && right->isLiteral && !floatFromVal (valFromType (rtype)))
    return left;

  /* if left is literal zero return right */
  if (IS_LITERAL (letype) && left->isLiteral && !floatFromVal (valFromType (ltype)))
    return right;

  /* if left is a pointer then size */
  if (IS_PTR (ltype) || IS_ARRAY(ltype))
    {
      isarray = left->isaddr;
      // there is no need to multiply with 1
      if (getSize (ltype->next) != 1)
        {
          size  = operandFromLit (getSize (ltype->next));
          SPEC_USIGN (getSpec (operandType (size))) = 1;
          indexUnsigned = IS_UNSIGNED (getSpec (operandType (right)));
          right = geniCodeMultiply (right, size, resultType);
          /* Even if right is a 'unsigned char',
             the result will be a 'signed int' due to the promotion rules.
             It doesn't make sense when accessing arrays, so let's fix it here: */
          if (indexUnsigned)
            SPEC_USIGN (getSpec (operandType (right))) = 1;
        }
      resType = copyLinkChain (ltype);
    }
  else
    { // make them the same size
      resType = usualBinaryConversions (&left, &right, resultType, '+');
    }

  /* if they are both literals then we know */
  if (IS_LITERAL (letype) && IS_LITERAL (retype)
      && left->isLiteral && right->isLiteral)
    return operandFromValue (valPlus (valFromType (ltype),
                                      valFromType (rtype)));

  ic = newiCode ('+', left, right);

  IC_RESULT (ic) = newiTempOperand (resType, 1);
  IC_RESULT (ic)->isaddr = (isarray ? 1 : 0);

  /* if left or right is a float then support
     routine */
  if (IS_FLOAT (ltype) || IS_FLOAT (rtype)
      || IS_FIXED (ltype) || IS_FIXED (rtype))
    ic->supportRtn = 1;

  ADDTOCHAIN (ic);

  return IC_RESULT (ic);
}

/*-----------------------------------------------------------------*/
/* aggrToPtr - changes an "aggregate" to a "pointer to aggregate"  */
/*-----------------------------------------------------------------*/
sym_link *
aggrToPtr (sym_link * type, bool force)
{
  sym_link *etype;
  sym_link *ptype;

  if (IS_PTR (type) && !force)
    return type;

  etype = getSpec (type);
  ptype = newLink (DECLARATOR);

  ptype->next = type;

  /* set the pointer depending on the storage class */
  DCL_TYPE (ptype) = PTR_TYPE (SPEC_OCLS (etype));
  return ptype;
}

/*------------------------------------------------------------------*/
/* aggrToPtrDclType - like aggrToPtr, but returns only the DCL_TYPE */
/*------------------------------------------------------------------*/
int
aggrToPtrDclType (sym_link * type, bool force)
{
  if (IS_PTR (type) && !force)
    return DCL_TYPE (type);

  /* return the pointer depending on the storage class */
  return PTR_TYPE (SPEC_OCLS (getSpec (type)));
}

/*-----------------------------------------------------------------*/
/* geniCodeArray2Ptr - array to pointer                            */
/*-----------------------------------------------------------------*/
static operand *
geniCodeArray2Ptr (operand * op)
{
  sym_link *optype = operandType (op);
  sym_link *opetype = getSpec (optype);

  /* set the pointer depending on the storage class */
  DCL_TYPE (optype) = PTR_TYPE (SPEC_OCLS (opetype));
  /* now remove the storage class from this itemp */
  SPEC_SCLS (opetype) = S_FIXED;
  SPEC_OCLS (opetype) = NULL;

  op->isaddr = 0;
  return op;
}


/*-----------------------------------------------------------------*/
/* geniCodeArray - array access                                    */
/*-----------------------------------------------------------------*/
static operand *
geniCodeArray (operand * left, operand * right, int lvl)
{
  iCode *ic;
  operand *size;
  sym_link *ltype = operandType (left);
  bool indexUnsigned;
  RESULT_TYPE resultType;

  resultType = (getArraySizePtr(left) >= INTSIZE) ? RESULT_TYPE_INT : RESULT_TYPE_CHAR;
  if (DCL_ELEM (ltype))
    {
      if (DCL_ELEM (ltype) * getSize (ltype->next) <= 255)
        resultType = RESULT_TYPE_CHAR;
    }

  if (IS_PTR (ltype))
    {
      if (IS_PTR (ltype->next) && left->isaddr)
        {
          left = geniCodeRValue (left, FALSE);
        }

      return geniCodeDerefPtr (geniCodeAdd (left, right, resultType, lvl),
                               lvl);
    }
  size = operandFromLit (getSize (ltype->next));
  SPEC_USIGN (getSpec (operandType (size))) = 1;
  indexUnsigned = IS_UNSIGNED (getSpec (operandType (right)));
  right = geniCodeMultiply (right, size, resultType);
  /* Even if right is a 'unsigned char', the result will be a 'signed int' due to the promotion rules.
     It doesn't make sense when accessing arrays, so let's fix it here: */
  if (indexUnsigned)
    SPEC_USIGN (getSpec (operandType (right))) = 1;
  /* we can check for limits here */
  /* already done in SDCCast.c
  if (isOperandLiteral (right) &&
      IS_ARRAY (ltype) &&
      DCL_ELEM (ltype) &&
      (operandLitValue (right) / getSize (ltype->next)) >= DCL_ELEM (ltype))
    {
      werror (W_IDX_OUT_OF_BOUNDS,
              (int) operandLitValue (right) / getSize (ltype->next),
              DCL_ELEM (ltype));
    }
  */

  ic = newiCode ('+', left, right);

  IC_RESULT (ic) = newiTempOperand (((IS_PTR (ltype) &&
                                      !IS_AGGREGATE (ltype->next) &&
                                      !IS_PTR (ltype->next))
                                     ? ltype : ltype->next), 0);

  if (!IS_AGGREGATE (ltype->next))
    {
      IC_RESULT (ic)->isaddr = 1;
      IC_RESULT (ic)->aggr2ptr = 1;
    }
  ADDTOCHAIN (ic);

  return IC_RESULT (ic);
}

/*-----------------------------------------------------------------*/
/* geniCodeStruct - generates intermediate code for structures     */
/*-----------------------------------------------------------------*/
operand *
geniCodeStruct (operand * left, operand * right, bool islval)
{
  iCode *ic;
  sym_link *type = operandType (left);
  sym_link *etype = getSpec (type);
  sym_link *retype;
  symbol *element = getStructElement (SPEC_STRUCT (etype),
                                      right->operand.symOperand);

  wassert(IS_SYMOP(right));

  wassert(IS_STRUCT (type) || (IS_PTR (type) && IS_STRUCT (type->next)));

  /* add the offset */
  ic = newiCode ('+', left, operandFromLit (element->offset));

  IC_RESULT (ic) = newiTempOperand (element->type, 0);

  /* preserve the storage & output class of the struct */
  /* as well as the volatile attribute */
  retype = getSpec (operandType (IC_RESULT (ic)));
  SPEC_SCLS (retype) = SPEC_SCLS (etype);
  SPEC_OCLS (retype) = SPEC_OCLS (etype);
  SPEC_VOLATILE (retype) |= SPEC_VOLATILE (etype);
  SPEC_CONST (retype) |= SPEC_CONST (etype);

  if (IS_PTR (element->type))
    setOperandType (IC_RESULT (ic), aggrToPtr (operandType (IC_RESULT (ic)), TRUE));

  IC_RESULT (ic)->isaddr = (!IS_AGGREGATE (element->type));

  ADDTOCHAIN (ic);
  return (islval ? IC_RESULT (ic) : geniCodeRValue (IC_RESULT (ic), TRUE));
}

/*-----------------------------------------------------------------*/
/* geniCodePostInc - generate int code for Post increment          */
/*-----------------------------------------------------------------*/
operand *
geniCodePostInc (operand * op)
{
  iCode *ic;
  operand *rOp;
  sym_link *optype = operandType (op);
  operand *result;
  operand *rv = (IS_ITEMP (op) ?
                 geniCodeRValue (op, (IS_PTR (optype) ? TRUE : FALSE)) :
                 op);
  sym_link *rvtype = operandType (rv);
  int size = 0;

  /* if this is not an address we have trouble */
  if (!op->isaddr)
    {
      werror (E_LVALUE_REQUIRED, "++");
      return op;
    }

  rOp = newiTempOperand (rvtype, 0);
  OP_SYMBOL(rOp)->noSpilLoc = 1;

  if (IS_ITEMP (rv))
    OP_SYMBOL(rv)->noSpilLoc = 1;

  geniCodeAssign (rOp, rv, 0, 0);

  size = (IS_PTR (rvtype) ? getSize (rvtype->next) : 1);
  if (size == 0)
    werror(W_SIZEOF_VOID);
  if (IS_FLOAT (rvtype))
    ic = newiCode ('+', rv, operandFromValue (constFloatVal ("1.0")));
  else if (IS_FIXED16X16 (rvtype))
    ic = newiCode ('+', rv, operandFromValue (constFixed16x16Val ("1.0")));
  else
    ic = newiCode ('+', rv, operandFromLit (size));

  IC_RESULT (ic) = result = newiTempOperand (rvtype, 0);
  ADDTOCHAIN (ic);

  geniCodeAssign (op, result, 0, 0);

  return rOp;

}

/*-----------------------------------------------------------------*/
/* geniCodePreInc - generate code for preIncrement                 */
/*-----------------------------------------------------------------*/
operand *
geniCodePreInc (operand * op, bool lvalue)
{
  iCode *ic;
  sym_link *optype = operandType (op);
  operand *rop = (IS_ITEMP (op) ?
                  geniCodeRValue (op, (IS_PTR (optype) ? TRUE : FALSE)) :
                  op);
  sym_link *roptype = operandType (rop);
  operand *result;
  int size = 0;

  if (!op->isaddr)
    {
      werror (E_LVALUE_REQUIRED, "++");
      return op;
    }

  size = (IS_PTR (roptype) ? getSize (roptype->next) : 1);
  if (size == 0)
    werror(W_SIZEOF_VOID);
  if (IS_FLOAT (roptype))
    ic = newiCode ('+', rop, operandFromValue (constFloatVal ("1.0")));
  else if (IS_FIXED16X16 (roptype))
    ic = newiCode ('+', rop, operandFromValue (constFixed16x16Val ("1.0")));
  else
    ic = newiCode ('+', rop, operandFromLit (size));
  IC_RESULT (ic) = result = newiTempOperand (roptype, 0);
  ADDTOCHAIN (ic);

  (void) geniCodeAssign (op, result, 0, 0);
  if (lvalue || IS_TRUE_SYMOP (op) || IS_BITVAR (optype))
    return op;
  else
    return result;
}

/*-----------------------------------------------------------------*/
/* geniCodePostDec - generates code for Post decrement             */
/*-----------------------------------------------------------------*/
operand *
geniCodePostDec (operand * op)
{
  iCode *ic;
  operand *rOp;
  sym_link *optype = operandType (op);
  operand *result;
  operand *rv = (IS_ITEMP (op) ?
                 geniCodeRValue (op, (IS_PTR (optype) ? TRUE : FALSE)) :
                 op);
  sym_link *rvtype = operandType (rv);
  int size = 0;

  /* if this is not an address we have trouble */
  if (!op->isaddr)
    {
      werror (E_LVALUE_REQUIRED, "--");
      return op;
    }

  rOp = newiTempOperand (rvtype, 0);
  OP_SYMBOL(rOp)->noSpilLoc = 1;

  if (IS_ITEMP (rv))
    OP_SYMBOL(rv)->noSpilLoc = 1;

  geniCodeAssign (rOp, rv, 0, 0);

  size = (IS_PTR (rvtype) ? getSize (rvtype->next) : 1);
  if (size == 0)
    werror(W_SIZEOF_VOID);
  if (IS_FLOAT (rvtype))
    ic = newiCode ('-', rv, operandFromValue (constFloatVal ("1.0")));
  else if (IS_FIXED16X16 (rvtype))
    ic = newiCode ('-', rv, operandFromValue (constFixed16x16Val ("1.0")));
  else
    ic = newiCode ('-', rv, operandFromLit (size));

  IC_RESULT (ic) = result = newiTempOperand (rvtype, 0);
  ADDTOCHAIN (ic);

  geniCodeAssign (op, result, 0, 0);

  return rOp;

}

/*-----------------------------------------------------------------*/
/* geniCodePreDec - generate code for pre  decrement               */
/*-----------------------------------------------------------------*/
operand *
geniCodePreDec (operand * op, bool lvalue)
{
  iCode *ic;
  sym_link *optype = operandType (op);
  operand *rop = (IS_ITEMP (op) ?
                  geniCodeRValue (op, (IS_PTR (optype) ? TRUE : FALSE)) :
                  op);
  sym_link *roptype = operandType (rop);
  operand *result;
  int size = 0;

  if (!op->isaddr)
    {
      werror (E_LVALUE_REQUIRED, "--");
      return op;
    }

  size = (IS_PTR (roptype) ? getSize (roptype->next) : 1);
  if (size == 0)
    werror(W_SIZEOF_VOID);
  if (IS_FLOAT (roptype))
    ic = newiCode ('-', rop, operandFromValue (constFloatVal ("1.0")));
  else if (IS_FIXED16X16 (roptype))
    ic = newiCode ('-', rop, operandFromValue (constFixed16x16Val ("1.0")));
  else
    ic = newiCode ('-', rop, operandFromLit (size));
  IC_RESULT (ic) = result = newiTempOperand (roptype, 0);
  ADDTOCHAIN (ic);

  (void) geniCodeAssign (op, result, 0, 0);
  if (lvalue || IS_TRUE_SYMOP (op) || IS_BITVAR (optype))
    return op;
  else
    return result;
}


/*-----------------------------------------------------------------*/
/* geniCodeBitwise - gen int code for bitWise operators            */
/*-----------------------------------------------------------------*/
operand *
geniCodeBitwise (operand * left, operand * right,
                 int oper, sym_link * resType)
{
  iCode *ic;

  left = geniCodeCast (resType, left, TRUE);
  right = geniCodeCast (resType, right, TRUE);

  ic = newiCode (oper, left, right);
  IC_RESULT (ic) = newiTempOperand (resType, 0);

  ADDTOCHAIN (ic);
  return IC_RESULT (ic);
}

/*-----------------------------------------------------------------*/
/* geniCodeAddressOf - gens icode for '&' address of operator      */
/*-----------------------------------------------------------------*/
operand *
geniCodeAddressOf (operand * op)
{
  iCode *ic;
  sym_link *p;
  sym_link *optype = operandType (op);
  sym_link *opetype = getSpec (optype);

  if (IS_ITEMP (op) && op->isaddr && IS_PTR (optype))
    {
      op = operandFromOperand (op);
      op->isaddr = 0;
      return op;
    }

  /* lvalue check already done in decorateType */
  /* this must be a lvalue */
/*     if (!op->isaddr && !IS_AGGREGATE(optype)) { */
/*  werror (E_LVALUE_REQUIRED,"&"); */
/*  return op; */
/*     } */

  p = newLink (DECLARATOR);

  /* set the pointer depending on the storage class */
  DCL_TYPE (p) = PTR_TYPE (SPEC_OCLS (opetype));

  p->next = copyLinkChain (optype);

  /* if already a temp */
  if (IS_ITEMP (op))
    {
      setOperandType (op, p);
      op->isaddr = 0;
      return op;
    }

  /* otherwise make this of the type coming in */
  ic = newiCode (ADDRESS_OF, op, NULL);
  IC_RESULT (ic) = newiTempOperand (p, 1);
  IC_RESULT (ic)->isaddr = 0;
  ADDTOCHAIN (ic);
  return IC_RESULT (ic);
}

/*-----------------------------------------------------------------*/
/* setOClass - sets the output class depending on the pointer type */
/*-----------------------------------------------------------------*/
void
setOClass (sym_link * ptr, sym_link * spec)
{
  switch (DCL_TYPE (ptr))
    {
    case POINTER:
      SPEC_OCLS (spec) = data;
      break;

    case GPOINTER:
      SPEC_OCLS (spec) = generic;
      break;

    case FPOINTER:
      SPEC_OCLS (spec) = xdata;
      break;

    case CPOINTER:
      SPEC_OCLS (spec) = code;
      break;

    case IPOINTER:
      SPEC_OCLS (spec) = idata;
      break;

    case PPOINTER:
      SPEC_OCLS (spec) = xstack;
      break;

    case EEPPOINTER:
      SPEC_OCLS (spec) = eeprom;
      break;

    default:
      break;
    }
}

/*-----------------------------------------------------------------*/
/* geniCodeDerefPtr - dereference pointer with '*'                 */
/*-----------------------------------------------------------------*/
operand *
geniCodeDerefPtr (operand * op,int lvl)
{
  sym_link *rtype, *retype;
  sym_link *optype = operandType (op);

  // if this is an array then array access
  if (IS_ARRAY (optype)) {
    // don't worry, this will be optimized out later
    return geniCodeArray (op, operandFromLit (0), lvl);
  }

  // just in case someone screws up
  wassert (IS_PTR (optype));

  if (IS_TRUE_SYMOP (op))
    {
      op->isaddr = 1;
      op = geniCodeRValue (op, TRUE);
    }

  /* now get rid of the pointer part */
  if (isLvaluereq(lvl) && IS_ITEMP (op))
    {
      retype = getSpec (rtype = copyLinkChain (optype));
    }
  else
    {
      retype = getSpec (rtype = copyLinkChain (optype->next));
      /* outputclass needs 2b updated */
      setOClass (optype, retype);
    }

  op->isGptr = IS_GENPTR (optype);

  op->isaddr = (IS_PTR (rtype) ||
                IS_STRUCT (rtype) ||
                IS_INT (rtype) ||
                IS_CHAR (rtype) ||
                IS_FLOAT (rtype) ||
                IS_FIXED (rtype));

  if (!isLvaluereq(lvl))
    op = geniCodeRValue (op, TRUE);

  setOperandType (op, rtype);

  return op;
}

/*-----------------------------------------------------------------*/
/* geniCodeUnaryMinus - does a unary minus of the operand          */
/*-----------------------------------------------------------------*/
operand *
geniCodeUnaryMinus (operand * op)
{
  iCode *ic;
  sym_link *optype = operandType (op);

  if (IS_LITERAL (optype))
    return operandFromLit (-floatFromVal (op->operand.valOperand));

  ic = newiCode (UNARYMINUS, op, NULL);
  IC_RESULT (ic) = newiTempOperand (optype, 0);
  ADDTOCHAIN (ic);
  return IC_RESULT (ic);
}

/*-----------------------------------------------------------------*/
/* geniCodeLeftShift - gen i code for left shift                   */
/*-----------------------------------------------------------------*/
operand *
geniCodeLeftShift (operand * left, operand * right, RESULT_TYPE resultType)
{
  iCode *ic;
  sym_link *resType;

  ic = newiCode (LEFT_OP, left, right);

  resType = usualBinaryConversions (&left, &right, resultType, LEFT_OP);
  IC_RESULT (ic) = newiTempOperand (resType, 0);
  ADDTOCHAIN (ic);
  return IC_RESULT (ic);
}

/*-----------------------------------------------------------------*/
/* geniCodeRightShift - gen i code for right shift                 */
/*-----------------------------------------------------------------*/
operand *
geniCodeRightShift (operand * left, operand * right)
{
  iCode *ic;

  ic = newiCode (RIGHT_OP, left, right);
  IC_RESULT (ic) = newiTempOperand (operandType (left), 0);
  ADDTOCHAIN (ic);
  return IC_RESULT (ic);
}

/*-----------------------------------------------------------------*/
/* geniCodeLogic- logic code                                       */
/*-----------------------------------------------------------------*/
static operand *
geniCodeLogic (operand * left, operand * right, int op, ast *tree)
{
  iCode *ic;
  sym_link *ctype, *ttype;
  sym_link *rtype = operandType (right);
  sym_link *ltype = operandType (left);

  /* left is integral type and right is literal then
     check if the literal value is within bounds */
  if (IS_INTEGRAL (ltype) && IS_VALOP (right) && IS_LITERAL (rtype))
    {
      CCR_RESULT ccr_result = checkConstantRange (ltype, rtype, op, FALSE);
      switch (ccr_result)
        {
          case CCR_ALWAYS_TRUE:
          case CCR_ALWAYS_FALSE:
            if (!options.lessPedantic)
              werror (W_COMP_RANGE, "true resp. false");
            return operandFromLit (ccr_result == CCR_ALWAYS_TRUE ? 1 : 0);
          default:
            break;
        }
    }

  /* if one operand is a pointer and the other is a literal generic void pointer,
     change the type of the literal generic void pointer to match the other pointer */
  if (IS_GENPTR (ltype) && IS_VOID (ltype->next) && IS_ITEMP (left)
      && IS_PTR (rtype) && !IS_GENPTR(rtype))
    {
      /* find left's definition */
      ic = (iCode *) setFirstItem (iCodeChain);
      while (ic)
        {
          if (((ic->op == CAST) || (ic->op == '='))
              && isOperandEqual(left, IC_RESULT (ic)))
            break;
          else
            ic = setNextItem (iCodeChain);
        }
      /* if casting literal to generic pointer, then cast to rtype instead */
      if (ic && (ic->op == CAST) && isOperandLiteral(IC_RIGHT (ic)))
        {
          left = operandFromValue (valCastLiteral (rtype, operandLitValue (IC_RIGHT (ic))));
          ltype = operandType(left);
        }
    }
  if (IS_GENPTR (rtype) && IS_VOID (rtype->next) && IS_ITEMP (right)
      && IS_PTR (ltype) && !IS_GENPTR(ltype))
    {
      /* find right's definition */
      ic = (iCode *) setFirstItem (iCodeChain);
      while (ic)
        {
          if (((ic->op == CAST) || (ic->op == '='))
              && isOperandEqual(right, IC_RESULT (ic)))
            break;
          else
            ic = setNextItem (iCodeChain);
        }
      /* if casting literal to generic pointer, then cast to rtype instead */
      if (ic && (ic->op == CAST) && isOperandLiteral(IC_RIGHT (ic)))
        {
          right = operandFromValue (valCastLiteral (ltype, operandLitValue (IC_RIGHT (ic))));
          rtype = operandType(right);
        }
    }

  ctype = usualBinaryConversions (&left, &right, RESULT_TYPE_BIT, 0);

  ic = newiCode (op, left, right);
  /* store 0 or 1 in result */
  ttype = (tree && IS_BIT (tree->ftype)) ? newBoolLink() : newCharLink();
  IC_RESULT (ic) = newiTempOperand (ttype, 1);

  /* if comparing float
     and not a '==' || '!=' || '&&' || '||' (these
     will be inlined */
  if (IS_FLOAT(ctype) &&
      op != EQ_OP &&
      op != NE_OP &&
      op != AND_OP &&
      op != OR_OP)
   ic->supportRtn = 1;

  /* if comparing a fixed type use support functions */
  if (IS_FIXED(ctype))
    ic->supportRtn = 1;

  ADDTOCHAIN (ic);
  return IC_RESULT (ic);
}

/*-----------------------------------------------------------------*/
/* geniCodeLogicAndOr - && || operations                           */
/*-----------------------------------------------------------------*/
static operand *
geniCodeLogicAndOr (ast *tree, int lvl)
{
  iCode *ic;
  sym_link *type;
  symbol *falseLabel = newiTempLabel (NULL);
  symbol *trueLabel  = newiTempLabel (NULL);
  symbol *exitLabel  = newiTempLabel (NULL);
  operand *op, *result, *condition;

  /* AND_OP and OR_OP are no longer generated because of bug-905492.
     They can be reenabled by executing the following block. If you find
     a decent optimization you could start right here:
  */
#if 0
  if (0)
    {
       operand *leftOp, *rightOp;

       leftOp  = geniCodeRValue (ast2iCode (tree->left , lvl + 1), FALSE);
       rightOp = geniCodeRValue (ast2iCode (tree->right, lvl + 1), FALSE);

       return geniCodeLogic (leftOp, rightOp, tree->opval.op);
    }
#endif

  /* generate two IFX for the '&&' or '||' op */

  /* evaluate left operand */
  condition = ast2iCode (tree->left, lvl + 1);
  op = geniCodeRValue (condition, FALSE);

  /* test left operand */
  if (tree->opval.op == AND_OP)
    ic = newiCodeCondition (op, NULL, falseLabel);
  else /* OR_OP */
    ic = newiCodeCondition (op, trueLabel, NULL);
  ADDTOCHAIN (ic);

  /* evaluate right operand */
  condition = ast2iCode (tree->right, lvl + 1);
  op = geniCodeRValue (condition, FALSE);

  /* test right operand */
  ic = newiCodeCondition (op, trueLabel, NULL);
  ADDTOCHAIN (ic);

  /* store 0 or 1 in result */
  type = (IS_BIT (tree->ftype)) ? newBoolLink() : newCharLink();
  result = newiTempOperand (type, 1);

  geniCodeLabel (falseLabel);
  geniCodeAssign (result, operandFromLit (0), 0, 0);
  /* generate an unconditional goto */
  geniCodeGoto (exitLabel);

  geniCodeLabel (trueLabel);
  geniCodeAssign (result, operandFromLit (1), 0, 0);

  geniCodeLabel (exitLabel);

  return result;
}

/*-----------------------------------------------------------------*/
/* geniCodeUnary - for a generic unary operation                   */
/*-----------------------------------------------------------------*/
operand *
geniCodeUnary (operand * op, int oper)
{
  iCode *ic = newiCode (oper, op, NULL);

  IC_RESULT (ic) = newiTempOperand (operandType (op), 0);
  ADDTOCHAIN (ic);
  return IC_RESULT (ic);
}

/*-----------------------------------------------------------------*/
/* geniCodeBinary - for a generic binary operation                 */
/*-----------------------------------------------------------------*/
operand *
geniCodeBinary (operand * left, operand * right, int oper)
{
  iCode *ic = newiCode (oper, left, right);

  IC_RESULT (ic) = newiTempOperand (operandType (left), 0);
  ADDTOCHAIN (ic);
  return IC_RESULT (ic);
}

/*-----------------------------------------------------------------*/
/* geniCodeConditional - geniCode for '?' ':' operation            */
/*-----------------------------------------------------------------*/
operand *
geniCodeConditional (ast * tree,int lvl)
{
  iCode *ic;
  symbol *falseLabel = newiTempLabel (NULL);
  symbol *exitLabel = newiTempLabel (NULL);
  ast *astTrue  = tree->right->left;
  ast *astFalse = tree->right->right;
  operand *cond = ast2iCode (tree->left, lvl+1);
  operand *result = newiTempOperand (tree->ftype, 0);
  operand *opTrue, *opFalse;

  ic = newiCodeCondition (geniCodeRValue (cond, FALSE), NULL, falseLabel);
  ADDTOCHAIN (ic);

  opTrue = ast2iCode (astTrue, lvl+1);

  /* move the value to the new operand */
  geniCodeAssign (result, geniCodeRValue (opTrue, FALSE), 0, 0);

  /* generate an unconditional goto */
  geniCodeGoto (exitLabel);

  /* now for the right side */
  geniCodeLabel (falseLabel);

  opFalse = ast2iCode (astFalse, lvl+1);
  geniCodeAssign (result, geniCodeRValue (opFalse, FALSE), 0, 0);

  /* create the exit label */
  geniCodeLabel (exitLabel);

  return result;
}

/*-----------------------------------------------------------------*/
/* checkTypes - check types for assignment                         */
/*-----------------------------------------------------------------*/
static operand *
checkTypes (operand * left, operand * right)
{
  sym_link *ltype = operandType (left);
  sym_link *rtype = operandType (right);

  /* left is integral type and right is literal then
     check if the literal value is within bounds */
  if (IS_INTEGRAL (ltype) && right->type == VALUE && IS_LITERAL (rtype) &&
      checkConstantRange (ltype, rtype, '=', FALSE) == CCR_OVL &&
      !options.lessPedantic)
    {
      werror (W_LIT_OVERFLOW);
    }

  /* if the left & right type don't exactly match */
  /* if pointer set then make sure the check is
     done with the type & not the pointer */
  /* then cast rights type to left */

  /* first check the type for pointer assignement */
  if (left->isaddr && IS_PTR (ltype) && IS_ITEMP (left) &&
      compareType (ltype, rtype) <= 0)
    {
      if (left->aggr2ptr)
        {
          right = geniCodeCast (ltype, right, TRUE);
          checkPtrQualifiers (ltype, rtype);
        }
      else
        {
          if (compareType (ltype->next, rtype) < 0)
            right = geniCodeCast (ltype->next, right, TRUE);
          checkPtrQualifiers (ltype->next, rtype);
        }
    }
  else
    {
      if (compareType (ltype, rtype) < 0)
        right = geniCodeCast (ltype, right, TRUE);
      checkPtrQualifiers (ltype, rtype);
    }
  return right;
}

/*-----------------------------------------------------------------*/
/* geniCodeAssign - generate code for assignment                   */
/*-----------------------------------------------------------------*/
operand *
geniCodeAssign (operand * left, operand * right, int nosupdate, int strictLval)
{
  iCode *ic;

  if (!left->isaddr && (!IS_ITEMP (left) || strictLval))
    {
      werror (E_LVALUE_REQUIRED, "assignment");
      return left;
    }

  right = checkTypes (left, right);

  /* If left is a true symbol & ! volatile
     create an assignment to temporary for
     the right & then assign this temporary
     to the symbol. This is SSA (static single
     assignment). Isn't it simple and folks have
     published mountains of paper on it */
  if (IS_TRUE_SYMOP (left) &&
      !isOperandVolatile (left, FALSE) &&
      isOperandGlobal (left))
    {
      symbol *sym = NULL;
      operand *newRight;
      sym_link *ltype = operandType (left);

      if (IS_TRUE_SYMOP (right))
        sym = OP_SYMBOL (right);
      ic = newiCode ('=', NULL, right);
      IC_RESULT (ic) = newRight = newiTempOperand (ltype, 0);
      /* avoid double fetch from volatile right, see bug 1369874 */
      if (!isOperandVolatile (right, FALSE))
        SPIL_LOC (newRight) = sym;
      right = newRight;
      ADDTOCHAIN (ic);
    }

  ic = newiCode ('=', NULL, right);
  IC_RESULT (ic) = left;
  ADDTOCHAIN (ic);

  /* if left isgptr flag is set then support
     routine will be required */
  if (left->isGptr)
    ic->supportRtn = 1;

  ic->nosupdate = nosupdate;
  /* left could be a pointer assignment,
     return the properly casted right instead */
  return right;
}

/*-----------------------------------------------------------------*/
/* geniCodeDummyRead - generate code for dummy read                */
/*-----------------------------------------------------------------*/
static void
geniCodeDummyRead (operand * op)
{
  iCode *ic;
  sym_link *type = operandType (op);

  if (!IS_VOLATILE(type))
    return;

  ic = newiCode (DUMMY_READ_VOLATILE, NULL, op);
  ADDTOCHAIN (ic);

  ic->nosupdate = 1;
}

/*-----------------------------------------------------------------*/
/* geniCodeSEParms - generate code for side effecting fcalls       */
/*-----------------------------------------------------------------*/
static void
geniCodeSEParms (ast * parms,int lvl)
{
  if (!parms)
    return;

  if (parms->type == EX_OP && parms->opval.op == PARAM)
    {
      geniCodeSEParms (parms->left,lvl);
      geniCodeSEParms (parms->right,lvl);
      return;
    }

  /* hack don't like this but too lazy to think of
     something better */
  if (IS_ADDRESS_OF_OP (parms))
    parms->left->lvalue = 1;

  if (IS_CAST_OP (parms) &&
      IS_PTR (parms->ftype) &&
      IS_ADDRESS_OF_OP (parms->right))
    parms->right->left->lvalue = 1;

  parms->opval.oprnd =
    geniCodeRValue (ast2iCode (parms,lvl+1), FALSE);

  parms->type = EX_OPERAND;
  AST_ARGREG(parms) = parms->etype ? SPEC_ARGREG(parms->etype) :
                SPEC_ARGREG(parms->ftype);
}

/*-----------------------------------------------------------------*/
/* geniCodeParms - generates parameters                            */
/*-----------------------------------------------------------------*/
value *
geniCodeParms (ast * parms, value *argVals, int *iArg, int *stack,
               sym_link * ftype, int lvl)
{
  iCode *ic;
  operand *pval;

  if (!parms)
    return argVals;

  /* if this is a param node then do the left & right */
  if (parms->type == EX_OP && parms->opval.op == PARAM)
    {
      argVals = geniCodeParms (parms->left, argVals, iArg, stack, ftype, lvl);
      argVals = geniCodeParms (parms->right, argVals, iArg, stack, ftype, lvl);
      return argVals;
    }

  /* get the parameter value */
  if (parms->type == EX_OPERAND)
    pval = parms->opval.oprnd;
  else
    {
      /* maybe this else should go away ?? */
      /* hack don't like this but too lazy to think of
         something better */
      if (IS_ADDRESS_OF_OP (parms))
        parms->left->lvalue = 1;

      if (IS_CAST_OP (parms) &&
          IS_PTR (parms->ftype) &&
          IS_ADDRESS_OF_OP (parms->right))
        parms->right->left->lvalue = 1;

      pval = geniCodeRValue (ast2iCode (parms, lvl+1), FALSE);
    }

  /* if register parm then make it a send */
  if ((IS_REGPARM (parms->etype) && !IFFUNC_HASVARARGS(ftype)) ||
      IFFUNC_ISBUILTIN(ftype))
    {
      pval = checkTypes (operandFromValue (argVals), pval);
      ic = newiCode (SEND, pval, NULL);
      ic->argreg = SPEC_ARGREG(parms->etype);
      ic->builtinSEND = FUNC_ISBUILTIN(ftype);
      ADDTOCHAIN (ic);
    }
  else
    {
      /* now decide whether to push or assign */
      if (!(options.stackAuto || IFFUNC_ISREENT (ftype)))
        {
          /* assign */
          operand *top = operandFromValue (argVals);
          /* clear useDef and other bitVectors */
          OP_USES (top) = OP_DEFS (top) = OP_SYMBOL (top)->clashes = NULL;
          geniCodeAssign (top, pval, 1, 0);
        }
      else
        {
          sym_link *p;
          if (argVals && (*iArg >= 0))
            {
              pval = checkTypes (operandFromValue (argVals), pval);
            }
          p = operandType (pval);
          /* push */
          ic = newiCode (IPUSH, pval, NULL);
          ic->parmPush = 1;
          /* update the stack adjustment */
          *stack += getSize (IS_AGGREGATE (p) ? aggrToPtr (p, FALSE) : p);
          ADDTOCHAIN (ic);
        }
    }

  if (*iArg >= 0)
    argVals = argVals->next;
  (*iArg)++;
  return argVals;
}

/*-----------------------------------------------------------------*/
/* geniCodeCall - generates temp code for calling                  */
/*-----------------------------------------------------------------*/
operand *
geniCodeCall (operand * left, ast * parms,int lvl)
{
  iCode *ic;
  operand *result;
  sym_link *type, *etype;
  sym_link *ftype;
  int stack = 0;
  int iArg = 0;

  ftype = operandType (left);
  if (!IS_FUNC(ftype) && !IS_FUNCPTR(ftype))
    {
      werror (E_FUNCTION_EXPECTED);
      return operandFromValue(valueFromLit(0));
    }

  /* take care of parameters with side-effecting
     function calls in them, this is required to take care
     of overlaying function parameters */
  geniCodeSEParms (parms, lvl);

  if (IS_FUNCPTR (ftype))
    ftype = ftype->next;

  /* first the parameters */
  if ((options.stackAuto || IFFUNC_ISREENT (ftype)) && !IFFUNC_ISBUILTIN (ftype))
    {
      value *argVals;
      int nArgs = 0;
      ast * parm;
      int nParms = 0;

      //count expected arguments except varargs
      for (argVals = FUNC_ARGS (ftype); argVals; argVals = argVals->next)
        nArgs++;
      //count actual parameters including varargs
      for (parm = parms;
           parm && parm->type == EX_OP && parm->opval.op == PARAM;
           parm = parm->right)
        {
          if (parm->left)
            nParms++;
        }
      if (parm)
        nParms++;
      argVals = FUNC_ARGS (ftype);
      iArg = nArgs - nParms;

      // reverse the argVals to match the parms
      argVals = reverseVal (argVals);
      geniCodeParms (parms, argVals, &iArg, &stack, ftype, lvl);
      argVals = reverseVal (argVals);
    }
  else
    {
      geniCodeParms (parms, FUNC_ARGS (ftype), &iArg, &stack, ftype, lvl);
    }

  /* now call : if symbol then pcall */
  if (IS_OP_POINTER (left) || IS_ITEMP(left)) {
    ic = newiCode (PCALL, left, NULL);
  } else {
    ic = newiCode (CALL, left, NULL);
  }

  type = copyLinkChain (ftype->next);
  etype = getSpec (type);
  SPEC_EXTR (etype) = 0;
  IC_RESULT (ic) = result = newiTempOperand (type, 1);

  ADDTOCHAIN (ic);

  /* stack adjustment after call */
  ic->parmBytes = stack;

  return result;
}

/*-----------------------------------------------------------------*/
/* geniCodeReceive - generate intermediate code for "receive"      */
/*-----------------------------------------------------------------*/
static void
geniCodeReceive (value * args, operand * func)
{
  unsigned char paramByteCounter = 0;

  /* for all arguments that are passed in registers */
  while (args)
    {
      if (IS_REGPARM (args->etype))
        {
          operand *opr = operandFromValue (args);
          operand *opl;
          symbol *sym = OP_SYMBOL (opr);
          iCode *ic;

          /* we will use it after all optimizations
             and before liveRange calculation */
          if (!sym->addrtaken && !IS_VOLATILE (sym->etype))
            {

              if ((IN_FARSPACE (SPEC_OCLS (sym->etype)) && !TARGET_IS_HC08) &&
                  options.stackAuto == 0 &&
                  (!(options.model == MODEL_FLAT24)) )
                {
                }
              else
                {
                  opl = newiTempOperand (args->type, 0);
                  sym->reqv = opl;
                  sym->reqv->key = sym->key;
                  OP_SYMBOL (sym->reqv)->key = sym->key;
                  OP_SYMBOL (sym->reqv)->isreqv = 1;
                  OP_SYMBOL (sym->reqv)->islocal = 0;
                  SPIL_LOC (sym->reqv) = sym;
                }
            }

          ic = newiCode (RECEIVE, func, NULL);
          ic->argreg = SPEC_ARGREG(args->etype);
          if (ic->argreg == 1) {
              currFunc->recvSize = getSize (sym->type);
          }
          IC_RESULT (ic) = opr;

          /* misuse of parmBytes (normally used for functions)
           * to save estimated stack position of this argument.
           * Normally this should be zero for RECEIVE iCodes.
           * No idea if this causes side effects on other ports. - dw
           */
          ic->parmBytes = paramByteCounter;

          /* what stack position do we have? */
          paramByteCounter += getSize (sym->type);

          ADDTOCHAIN (ic);
        }

      args = args->next;
    }
}

/*-----------------------------------------------------------------*/
/* geniCodeFunctionBody - create the function body                 */
/*-----------------------------------------------------------------*/
void
geniCodeFunctionBody (ast * tree,int lvl)
{
  iCode *ic;
  operand *func;
  sym_link *fetype;
  char *savefilename;
  int savelineno;

  /* reset the auto generation */
  /* numbers */
  iTempNum = 0;
  iTempLblNum = 0;
  operandKey = 0;
  iCodeKey = 0;
  func = ast2iCode (tree->left,lvl+1);
  fetype = getSpec (operandType (func));

  savefilename = filename;
  savelineno = lineno;
  filename = OP_SYMBOL (func)->fileDef;
  lineno = OP_SYMBOL (func)->lineDef;
  /* create an entry label */
  geniCodeLabel (entryLabel);
  filename = savefilename;
  lineno = savelineno;

  /* create a proc icode */
  ic = newiCode (FUNCTION, func, NULL);
  filename = ic->filename = OP_SYMBOL (func)->fileDef;
  lineno = ic->lineno = OP_SYMBOL (func)->lineDef;
  ic->tree = tree;

  ADDTOCHAIN (ic);

  /* for all parameters that are passed
     on registers add a "receive" */
  geniCodeReceive (tree->values.args, func);

  /* generate code for the body */
  ast2iCode (tree->right,lvl+1);

  /* create a label for return */
  geniCodeLabel (returnLabel);

  /* now generate the end proc */
  ic = newiCode (ENDFUNCTION, func, NULL);
  ic->tree = tree;
  ADDTOCHAIN (ic);
  return;
}

/*-----------------------------------------------------------------*/
/* geniCodeReturn - gen icode for 'return' statement               */
/*-----------------------------------------------------------------*/
void
geniCodeReturn (operand * op)
{
  iCode *ic;

  /* if the operand is present force an rvalue */
  if (op)
    op = geniCodeRValue (op, FALSE);

  ic = newiCode (RETURN, op, NULL);
  ADDTOCHAIN (ic);
}

/*-----------------------------------------------------------------*/
/* geniCodeIfx - generates code for extended if statement          */
/*-----------------------------------------------------------------*/
void
geniCodeIfx (ast * tree,int lvl)
{
  iCode *ic;
  operand *condition = ast2iCode (tree->left,lvl+1);
  sym_link *cetype;

  /* if condition is null then exit */
  if (!condition)
    goto exit;
  else
    condition = geniCodeRValue (condition, FALSE);

  cetype = getSpec (operandType (condition));
  /* if the condition is a literal */
  if (IS_LITERAL (cetype))
    {
      if (floatFromVal (condition->operand.valOperand))
        {
          if (tree->trueLabel)
            geniCodeGoto (tree->trueLabel);
          else
            assert (0);
        }
      else
        {
          if (tree->falseLabel)
            geniCodeGoto (tree->falseLabel);
        }
      goto exit;
    }

  if (tree->trueLabel)
    {
      ic = newiCodeCondition (condition,
                              tree->trueLabel,
                              NULL);
      ADDTOCHAIN (ic);

      if (tree->falseLabel)
        geniCodeGoto (tree->falseLabel);
    }
  else
    {
      ic = newiCodeCondition (condition,
                              NULL,
                              tree->falseLabel);
      ADDTOCHAIN (ic);
    }

exit:
  ast2iCode (tree->right,lvl+1);
}

/*-----------------------------------------------------------------*/
/* geniCodeJumpTable - tries to create a jump table for switch     */
/*-----------------------------------------------------------------*/
int
geniCodeJumpTable (operand * cond, value * caseVals, ast * tree)
{
  int min, max, cnt = 1;
  int i, t;
  value *vch;
  iCode *ic;
  operand *boundary;
  symbol *falseLabel;
  set *labels = NULL;
  int needRangeCheck = !optimize.noJTabBoundary
                       || tree->values.switchVals.swDefault;
  sym_link *cetype = getSpec (operandType (cond));
  int sizeofMinCost, sizeofZeroMinCost, sizeofMaxCost;
  int sizeofMatchJump, sizeofJumpTable;
  int sizeIndex;

  if (!tree || !caseVals)
    return 0;

  /* the criteria for creating a jump table is */
  /* all integer numbers between the maximum & minimum must */
  /* be present , the maximum value should not exceed 255 */
  /* If not all integer numbers are present the algorithm */
  /* inserts jumps to the default label for the missing numbers */
  /* and decides later whether it is worth it */
  min = (int) ulFromVal (vch = caseVals);

  while (vch->next)
    {
      cnt++;
      vch = vch->next;
    }
  max = (int) ulFromVal (vch);

  /* Exit if the range is too large to handle with a jump table. */
  if (1 + max - min > port->jumptableCost.maxCount)
    return 0;

  switch (getSize (operandType (cond)))
    {
    case 1: sizeIndex = 0; break;
    case 2: sizeIndex = 1; break;
    case 4: sizeIndex = 2; break;
    default: return 0;
    }

  /* Compute the size cost of the range check and subtraction. */
  sizeofMinCost = 0;
  sizeofZeroMinCost = 0;
  sizeofMaxCost = 0;
  if (needRangeCheck)
    {
      if (!(min==0 && IS_UNSIGNED (cetype)))
        sizeofMinCost = port->jumptableCost.sizeofRangeCompare[sizeIndex];
      if (!IS_UNSIGNED (cetype))
        sizeofZeroMinCost = port->jumptableCost.sizeofRangeCompare[sizeIndex];
      sizeofMaxCost = port->jumptableCost.sizeofRangeCompare[sizeIndex];
    }
  if (min)
    sizeofMinCost += port->jumptableCost.sizeofSubtract;

  /* If the size cost of handling a non-zero minimum exceeds the */
  /* cost of extending the range down to zero, then it might be */
  /* better to extend the range to zero. */
  if (min > 0 && (sizeofMinCost-sizeofZeroMinCost)
                 >= (min * port->jumptableCost.sizeofElement))
    {
      /* Only extend the jump table if it would still be manageable. */
      if (1 + max <= port->jumptableCost.maxCount)
        {
          min = 0;
          if (IS_UNSIGNED (cetype))
            sizeofMinCost = 0;
          else
            sizeofMinCost = port->jumptableCost.sizeofRangeCompare[sizeIndex];
        }
    }

  /* Compute the total size cost of a jump table. */
  sizeofJumpTable = (1 + max - min) * port->jumptableCost.sizeofElement
                     + port->jumptableCost.sizeofDispatch
                     + sizeofMinCost + sizeofMaxCost;

  /* Compute the total size cost of a match & jump sequence */
  sizeofMatchJump = cnt * port->jumptableCost.sizeofMatchJump[sizeIndex];

  /* If the size cost of the jump table is uneconomical then exit */
  if (sizeofMatchJump <  sizeofJumpTable)
    return 0;

  /* The jump table is preferable. */

  /* First, a label for the default or missing cases. */
  if (tree->values.switchVals.swDefault)
    {
      SNPRINTF (buffer, sizeof(buffer),
                "_default_%d%s",
                tree->values.switchVals.swNum,
                tree->values.switchVals.swSuffix? tree->values.switchVals.swSuffix: "");
    }
  else
    {
      SNPRINTF (buffer, sizeof(buffer),
                "_swBrk_%d%s",
                tree->values.switchVals.swNum,
                tree->values.switchVals.swSuffix? tree->values.switchVals.swSuffix: "");
    }
  falseLabel = newiTempLabel (buffer);

  /* Build the list of labels for the jump table. */
  vch = caseVals;
  t = (int) ulFromVal (vch);
  for (i=min; i<=max; i++)
    {
      if (vch && t==i)
        {
          /* Explicit case: make a new label for it. */
          SNPRINTF (buffer, sizeof(buffer),
                    "_case_%d_%d%s",
                    tree->values.switchVals.swNum,
                    i,
		    tree->values.switchVals.swSuffix? tree->values.switchVals.swSuffix: "");
          addSet (&labels, newiTempLabel (buffer));
          vch = vch->next;
          if (vch)
            t = (int) ulFromVal (vch);
        }
      else
        {
          /* Implicit case: use the default label. */
          addSet (&labels, falseLabel);
        }
    }

  /* first we rule out the boundary conditions */
  /* if only optimization says so */
  if (needRangeCheck)
    {
      sym_link *cetype = getSpec (operandType (cond));
      /* no need to check the lower bound if
         the condition is unsigned & minimum value is zero */
      if (!(min == 0 && IS_UNSIGNED (cetype)))
        {
          boundary = geniCodeLogic (cond, operandFromLit (min), '<', NULL);
          ic = newiCodeCondition (boundary, falseLabel, NULL);
          ADDTOCHAIN (ic);
        }

      /* now for upper bounds */
      boundary = geniCodeLogic (cond, operandFromLit (max), '>', NULL);
      ic = newiCodeCondition (boundary, falseLabel, NULL);
      ADDTOCHAIN (ic);
    }

  /* if the min is not zero then we no make it zero */
  if (min)
    {
      cond = geniCodeSubtract (cond, operandFromLit (min), RESULT_TYPE_CHAR);
      if (!IS_LITERAL(getSpec(operandType(cond))))
        setOperandType (cond, UCHARTYPE);
    }

  /* now create the jumptable */
  ic = newiCode (JUMPTABLE, NULL, NULL);
  IC_JTCOND (ic) = cond;
  IC_JTLABELS (ic) = labels;
  ADDTOCHAIN (ic);
  return 1;
}

/*-----------------------------------------------------------------*/
/* geniCodeSwitch - changes a switch to a if statement             */
/*-----------------------------------------------------------------*/
void
geniCodeSwitch (ast * tree,int lvl)
{
  iCode *ic;
  operand *cond = geniCodeRValue (ast2iCode (tree->left,lvl+1), FALSE);
  value *caseVals = tree->values.switchVals.swVals;
  symbol *trueLabel, *falseLabel;

  /* If the condition is a literal, then just jump to the */
  /* appropriate case label. */
  if (IS_LITERAL(getSpec(operandType(cond))))
    {
      int switchVal, caseVal;

      switchVal = (int) ulFromVal (cond->operand.valOperand);
      while (caseVals)
        {
          caseVal = (int) ulFromVal (caseVals);
          if (caseVal == switchVal)
            {
              SNPRINTF (buffer, sizeof(buffer), "_case_%d_%d%s",
                        tree->values.switchVals.swNum, caseVal,
			tree->values.switchVals.swSuffix? tree->values.switchVals.swSuffix: "");
              trueLabel = newiTempLabel (buffer);
              geniCodeGoto (trueLabel);
              goto jumpTable;
            }
          caseVals = caseVals->next;
        }
      goto defaultOrBreak;
    }

  /* If cond is volatile, it might change while we are trying to */
  /* find the matching case. To avoid this possibility, make a   */
  /* non-volatile copy to use instead. */
  if (IS_OP_VOLATILE (cond))
    {
      operand * newcond;
      iCode * ic;

      newcond = newiTempOperand (operandType (cond), TRUE);
      newcond->isvolatile = 0;
      ic = newiCode ('=', NULL, cond);
      IC_RESULT (ic) = newcond;
      ADDTOCHAIN (ic);
      cond = newcond;
    }

  /* if we can make this a jump table */
  if (geniCodeJumpTable (cond, caseVals, tree))
    goto jumpTable;             /* no need for the comparison */

  /* for the cases defined do */
  while (caseVals)
    {

      operand *compare = geniCodeLogic (cond,
                                        operandFromValue (caseVals),
                                        EQ_OP, NULL);

      SNPRINTF (buffer, sizeof(buffer), "_case_%d_%d%s",
               tree->values.switchVals.swNum,
               (int) ulFromVal (caseVals),
	       tree->values.switchVals.swSuffix? tree->values.switchVals.swSuffix: "");
      trueLabel = newiTempLabel (buffer);

      ic = newiCodeCondition (compare, trueLabel, NULL);
      ADDTOCHAIN (ic);
      caseVals = caseVals->next;
    }


defaultOrBreak:
  /* if default is present then goto break else break */
  if (tree->values.switchVals.swDefault)
    {
        SNPRINTF (buffer, sizeof(buffer), "_default_%d%s", tree->values.switchVals.swNum,
                  tree->values.switchVals.swSuffix? tree->values.switchVals.swSuffix: "");
    }
  else
    {
        SNPRINTF (buffer, sizeof(buffer), "_swBrk_%d%s", tree->values.switchVals.swNum,
                  tree->values.switchVals.swSuffix? tree->values.switchVals.swSuffix: "");
    }

  falseLabel = newiTempLabel (buffer);
  geniCodeGoto (falseLabel);

jumpTable:
  ast2iCode (tree->right,lvl+1);
}

/*-----------------------------------------------------------------*/
/* geniCodeInline - intermediate code for inline assembler         */
/*-----------------------------------------------------------------*/
static void
geniCodeInline (ast * tree)
{
  iCode *ic;

  ic = newiCode (INLINEASM, NULL, NULL);
  IC_INLINE (ic) = tree->values.inlineasm;
  ADDTOCHAIN (ic);
}

/*-----------------------------------------------------------------*/
/* geniCodeArrayInit - intermediate code for array initializer     */
/*-----------------------------------------------------------------*/
static void
geniCodeArrayInit (ast * tree, operand *array)
{
  iCode *ic;

  if (!getenv("TRY_THE_NEW_INITIALIZER")) {
    ic = newiCode (ARRAYINIT, array, NULL);
    IC_ARRAYILIST (ic) = tree->values.constlist;
  } else {
    operand *left=newOperand(), *right=newOperand();
    left->type=right->type=SYMBOL;
    OP_SYMBOL(left)=AST_SYMBOL(tree->left);
    OP_SYMBOL(right)=AST_SYMBOL(tree->right);
    ic = newiCode (ARRAYINIT, left, right);
  }
  ADDTOCHAIN (ic);
}

/*-----------------------------------------------------------------*/
/* geniCodeCritical - intermediate code for a critical statement   */
/*-----------------------------------------------------------------*/
static void
geniCodeCritical (ast *tree, int lvl)
{
  iCode *ic;
  operand *op = NULL;
  sym_link *type;

  if (!options.stackAuto && !TARGET_IS_HC08)
    {
      type = newLink(SPECIFIER);
      SPEC_VOLATILE(type) = 1;
      SPEC_NOUN(type) = V_BIT;
      SPEC_SCLS(type) = S_BIT;
      SPEC_BLEN(type) = 1;
      SPEC_BSTR(type) = 0;
      op = newiTempOperand(type, 1);
    }

  /* If op is NULL, the original interrupt state will saved on */
  /* the stack. Otherwise, it will be saved in op. */

  /* Generate a save of the current interrupt state & disable */
  ic = newiCode (CRITICAL, NULL, NULL);
  IC_RESULT (ic) = op;
  ADDTOCHAIN (ic);

  /* Generate the critical code sequence */
  if (tree->left && tree->left->type == EX_VALUE)
    geniCodeDummyRead (ast2iCode (tree->left,lvl+1));
  else
    ast2iCode (tree->left,lvl+1);

  /* Generate a restore of the original interrupt state */
  ic = newiCode (ENDCRITICAL, NULL, op);
  ADDTOCHAIN (ic);
}

/*-----------------------------------------------------------------*/
/* Stuff used in ast2iCode to modify geniCodeDerefPtr in some      */
/* particular case. Ie : assigning or dereferencing array or ptr   */
/*-----------------------------------------------------------------*/
set * lvaluereqSet = NULL;
typedef struct lvalItem
  {
    int req;
    int lvl;
  }
lvalItem;

/*-----------------------------------------------------------------*/
/* addLvaluereq - add a flag for lvalreq for current ast level     */
/*-----------------------------------------------------------------*/
void addLvaluereq(int lvl)
{
  lvalItem * lpItem = (lvalItem *)Safe_alloc ( sizeof (lvalItem));
  lpItem->req=1;
  lpItem->lvl=lvl;
  addSetHead(&lvaluereqSet,lpItem);

}
/*-----------------------------------------------------------------*/
/* delLvaluereq - del a flag for lvalreq for current ast level     */
/*-----------------------------------------------------------------*/
void delLvaluereq()
{
  lvalItem * lpItem;
  lpItem = getSet(&lvaluereqSet);
  if(lpItem) Safe_free(lpItem);
}
/*-----------------------------------------------------------------*/
/* clearLvaluereq - clear lvalreq flag                             */
/*-----------------------------------------------------------------*/
void clearLvaluereq()
{
  lvalItem * lpItem;
  lpItem = peekSet(lvaluereqSet);
  if(lpItem) lpItem->req = 0;
}
/*-----------------------------------------------------------------*/
/* getLvaluereq - get the last lvalreq level                       */
/*-----------------------------------------------------------------*/
int getLvaluereqLvl()
{
  lvalItem * lpItem;
  lpItem = peekSet(lvaluereqSet);
  if(lpItem) return lpItem->lvl;
  return 0;
}
/*-----------------------------------------------------------------*/
/* isLvaluereq - is lvalreq valid for this level ?                 */
/*-----------------------------------------------------------------*/
int isLvaluereq(int lvl)
{
  lvalItem * lpItem;
  lpItem = peekSet(lvaluereqSet);
  if(lpItem) return ((lpItem->req)&&(lvl <= (lpItem->lvl+1)));
  return 0;
}

/*-----------------------------------------------------------------*/
/* ast2iCode - creates an icodeList from an ast                    */
/*-----------------------------------------------------------------*/
operand *
ast2iCode (ast * tree, int lvl)
{
  operand *left = NULL;
  operand *right = NULL;
  if (!tree)
    return NULL;

  /* set the global variables for filename & line number */
  if (tree->filename)
    filename = tree->filename;
  if (tree->lineno)
    lineno = tree->lineno;
  if (tree->block)
    block = tree->block;
  if (tree->level)
    scopeLevel = tree->level;
  if (tree->seqPoint)
    seqPoint = tree->seqPoint;

  if (tree->type == EX_VALUE)
    return operandFromValue (tree->opval.val);

  if (tree->type == EX_LINK)
    return operandFromLink (tree->opval.lnk);

  /* if we find a nullop */
  if (tree->type == EX_OP &&
     (tree->opval.op == NULLOP ||
     tree->opval.op == BLOCK))
    {
      if (tree->left && tree->left->type == EX_VALUE)
        geniCodeDummyRead (ast2iCode (tree->left, lvl+1));
      else
        ast2iCode (tree->left, lvl+1);
      if (tree->right && tree->right->type == EX_VALUE)
        geniCodeDummyRead (ast2iCode (tree->right, lvl+1));
      else
        ast2iCode (tree->right, lvl+1);
      return NULL;
    }

  /* special cases for not evaluating */
  if (tree->opval.op != ':' &&
      tree->opval.op != '?' &&
      tree->opval.op != CALL &&
      tree->opval.op != IFX &&
      tree->opval.op != AND_OP &&
      tree->opval.op != OR_OP &&
      tree->opval.op != LABEL &&
      tree->opval.op != GOTO &&
      tree->opval.op != SWITCH &&
      tree->opval.op != FUNCTION &&
      tree->opval.op != INLINEASM &&
      tree->opval.op != CRITICAL)
    {
        if (IS_ASSIGN_OP (tree->opval.op) ||
            IS_DEREF_OP (tree) ||
            IS_ADDRESS_OF_OP (tree) )
          {
            addLvaluereq(lvl);
            if ((IS_ARRAY_OP (tree->left) && IS_ARRAY_OP (tree->left->left)) ||
                (IS_DEREF_OP (tree) && IS_ARRAY_OP (tree->left)))
              clearLvaluereq();

            left = operandFromAst (tree->left, lvl);
            delLvaluereq();
            if (IS_DEREF_OP (tree) && IS_DEREF_OP (tree->left))
              left = geniCodeRValue (left, TRUE);
          }
        else
          {
            left = operandFromAst (tree->left, lvl);
          }
        if (tree->opval.op == INC_OP ||
            tree->opval.op == DEC_OP)
          {
            addLvaluereq(lvl);
            right = operandFromAst (tree->right, lvl);
            delLvaluereq();
          }
        else
          {
            right = operandFromAst (tree->right, lvl);
          }
      }

  /* now depending on the type of operand */
  /* this will be a biggy                 */
  switch (tree->opval.op)
    {
    case '[':                   /* array operation */
      {
        //sym_link *ltype = operandType (left);
        //left = geniCodeRValue (left, IS_PTR (ltype->next) ? TRUE : FALSE);
        left = geniCodeRValue (left, FALSE);
        right = geniCodeRValue (right, TRUE);
      }

      return geniCodeArray (left, right,lvl);

    case '.':                   /* structure dereference */
      if (IS_PTR (operandType (left)))
        left = geniCodeRValue (left, TRUE);
      else
        left = geniCodeRValue (left, FALSE);

      return geniCodeStruct (left, right, tree->lvalue);

    case PTR_OP:                /* structure pointer dereference */
      {
        sym_link *pType;
        pType = operandType (left);
        left = geniCodeRValue (left, TRUE);

        setOClass (pType, getSpec (operandType (left)));
      }

      return geniCodeStruct (left, right, tree->lvalue);

    case INC_OP:                /* increment operator */
      if (left)
        return geniCodePostInc (left);
      else
        return geniCodePreInc (right, tree->lvalue);

    case DEC_OP:                /* decrement operator */
      if (left)
        return geniCodePostDec (left);
      else
        return geniCodePreDec (right, tree->lvalue);

    case '&':                   /* bitwise and or address of operator */
      if (right)
        {                       /* this is a bitwise operator   */
          left = geniCodeRValue (left, FALSE);
          right = geniCodeRValue (right, FALSE);
          return geniCodeBitwise (left, right, BITWISEAND, tree->ftype);
        }
      else
        return geniCodeAddressOf (left);

    case '|':                   /* bitwise or & xor */
    case '^':
      return geniCodeBitwise (geniCodeRValue (left, FALSE),
                              geniCodeRValue (right, FALSE),
                              tree->opval.op,
                              tree->ftype);

    case '/':
      return geniCodeDivision (geniCodeRValue (left, FALSE),
                               geniCodeRValue (right, FALSE),
                               getResultTypeFromType (tree->ftype));

    case '%':
      return geniCodeModulus (geniCodeRValue (left, FALSE),
                              geniCodeRValue (right, FALSE),
                              getResultTypeFromType (tree->ftype));
    case '*':
      if (right)
        return geniCodeMultiply (geniCodeRValue (left, FALSE),
                                 geniCodeRValue (right, FALSE),
                                 getResultTypeFromType (tree->ftype));
      else
        return geniCodeDerefPtr (geniCodeRValue (left, FALSE),lvl);

    case '-':
      if (right)
        return geniCodeSubtract (geniCodeRValue (left, FALSE),
                                 geniCodeRValue (right, FALSE),
                                 getResultTypeFromType (tree->ftype));
      else
        return geniCodeUnaryMinus (geniCodeRValue (left, FALSE));

    case '+':
      if (right)
        return geniCodeAdd (geniCodeRValue (left, FALSE),
                            geniCodeRValue (right, FALSE),
                            getResultTypeFromType (tree->ftype),
                            lvl);
      else
        return geniCodeRValue (left, FALSE);    /* unary '+' has no meaning */

    case LEFT_OP:
      return geniCodeLeftShift (geniCodeRValue (left, FALSE),
                                geniCodeRValue (right, FALSE),
                                getResultTypeFromType (tree->ftype));

    case RIGHT_OP:
      return geniCodeRightShift (geniCodeRValue (left, FALSE),
                                 geniCodeRValue (right, FALSE));
    case CAST:
#if 0 // this indeed needs a second thought
      {
        operand *op;

        // let's keep this simple: get the rvalue we need
        op=geniCodeRValue (right, FALSE);
        // now cast it to whatever we want
        op=geniCodeCast (operandType(left), op, FALSE);
        // if this is going to be used as an lvalue, make it so
        if (tree->lvalue) {
          op->isaddr=1;
        }
        return op;
      }
#else // bug #604575, is it a bug ????
      return geniCodeCast (operandType (left),
                           geniCodeRValue (right, FALSE), FALSE);
#endif

    case '~':
    case RRC:
    case RLC:
    case SWAP:
      return geniCodeUnary (geniCodeRValue (left, FALSE), tree->opval.op);

    case '!':
    case GETHBIT:
      {
        operand *op = geniCodeUnary (geniCodeRValue (left, FALSE), tree->opval.op);
        if (!IS_BIT (operandType (op)))
          setOperandType (op, UCHARTYPE);
        return op;
      }
    case GETABIT:
      {
        operand *op = geniCodeBinary (geniCodeRValue (left, FALSE),
                                      geniCodeRValue (right, FALSE),
                                      tree->opval.op);
        if (!IS_BIT (operandType (op)))
          setOperandType (op, UCHARTYPE);
        return op;
      }
    case GETBYTE:
      {
        operand *op = geniCodeBinary (geniCodeRValue (left, FALSE),
                                      geniCodeRValue (right, FALSE),
                                      tree->opval.op);
        setOperandType (op, UCHARTYPE);
        return op;
      }
    case GETWORD:
      {
        operand *op = geniCodeBinary (geniCodeRValue (left, FALSE),
                                      geniCodeRValue (right, FALSE),
                                      tree->opval.op);
        setOperandType (op, UINTTYPE);
        return op;
      }
    case AND_OP:
    case OR_OP:
      return geniCodeLogicAndOr (tree, lvl);
    case '>':
    case '<':
    case LE_OP:
    case GE_OP:
    case EQ_OP:
    case NE_OP:
      /* different compilers (even different gccs) evaluate
         the two calls in a different order. to get the same
         result on all machines we have to specify a clear sequence.
      return geniCodeLogic (geniCodeRValue (left, FALSE),
                            geniCodeRValue (right, FALSE),
                            tree->opval.op);
      */
      {
                operand *leftOp, *rightOp;

        leftOp  = geniCodeRValue (left , FALSE);
        rightOp = geniCodeRValue (right, FALSE);

        return geniCodeLogic (leftOp, rightOp, tree->opval.op, tree);
      }
    case '?':
      return geniCodeConditional (tree, lvl);

    case SIZEOF:
      return operandFromLit (getSize (tree->right->ftype));

    case '=':
      {
        sym_link *rtype = operandType (right);
        sym_link *ltype = operandType (left);
        if (IS_PTR (rtype) && IS_ITEMP (right)
            && right->isaddr && compareType (rtype->next, ltype) == 1)
          right = geniCodeRValue (right, TRUE);
        else
          right = geniCodeRValue (right, FALSE);

        return geniCodeAssign (left, right, 0, 1);
      }
    case MUL_ASSIGN:
      return
        geniCodeAssign (left,
                geniCodeMultiply (geniCodeRValue (operandFromOperand (left),
                                                  FALSE),
                                  geniCodeRValue (right, FALSE),
                                  getResultTypeFromType (tree->ftype)),
                        0, 1);

    case DIV_ASSIGN:
      return
        geniCodeAssign (left,
                geniCodeDivision (geniCodeRValue (operandFromOperand (left),
                                                  FALSE),
                                  geniCodeRValue (right, FALSE),
                                  getResultTypeFromType (tree->ftype)),
                        0, 1);
    case MOD_ASSIGN:
      return
        geniCodeAssign (left,
                 geniCodeModulus (geniCodeRValue (operandFromOperand (left),
                                                  FALSE),
                                  geniCodeRValue (right, FALSE),
                                  getResultTypeFromType (tree->ftype)),
                        0, 1);
    case ADD_ASSIGN:
      {
        sym_link *rtype = operandType (right);
        sym_link *ltype = operandType (left);
        if (IS_PTR (rtype) && IS_ITEMP (right)
            && right->isaddr && compareType (rtype->next, ltype) == 1)
          right = geniCodeRValue (right, TRUE);
        else
          right = geniCodeRValue (right, FALSE);


        return geniCodeAssign (left,
                     geniCodeAdd (geniCodeRValue (operandFromOperand (left),
                                                  FALSE),
                                  right,
                                  getResultTypeFromType (tree->ftype),
                                  lvl),
                               0, 1);
      }
    case SUB_ASSIGN:
      {
        sym_link *rtype = operandType (right);
        sym_link *ltype = operandType (left);
        if (IS_PTR (rtype) && IS_ITEMP (right)
            && right->isaddr && compareType (rtype->next, ltype) == 1)
          {
            right = geniCodeRValue (right, TRUE);
          }
        else
          {
            right = geniCodeRValue (right, FALSE);
          }
        return
          geniCodeAssign (left,
                geniCodeSubtract (geniCodeRValue (operandFromOperand (left),
                                                  FALSE),
                                  right,
                                  getResultTypeFromType (tree->ftype)),
                          0, 1);
      }
    case LEFT_ASSIGN:
      return
        geniCodeAssign (left,
                geniCodeLeftShift (geniCodeRValue (operandFromOperand (left)
                                                   ,FALSE),
                                   geniCodeRValue (right, FALSE),
                                   getResultTypeFromType (tree->ftype)),
                        0, 1);
    case RIGHT_ASSIGN:
      return
        geniCodeAssign (left,
               geniCodeRightShift (geniCodeRValue (operandFromOperand (left)
                                                   ,FALSE),
                                   geniCodeRValue (right, FALSE)), 0, 1);
    case AND_ASSIGN:
      return
        geniCodeAssign (left,
                 geniCodeBitwise (geniCodeRValue (operandFromOperand (left),
                                                  FALSE),
                                  geniCodeRValue (right, FALSE),
                                  BITWISEAND,
                                  operandType (left)), 0, 1);
    case XOR_ASSIGN:
      return
        geniCodeAssign (left,
                 geniCodeBitwise (geniCodeRValue (operandFromOperand (left),
                                                  FALSE),
                                  geniCodeRValue (right, FALSE),
                                  '^',
                                  operandType (left)), 0, 1);
    case OR_ASSIGN:
      return
        geniCodeAssign (left,
                  geniCodeBitwise (geniCodeRValue (operandFromOperand (left)
                                                   ,FALSE),
                                   geniCodeRValue (right, FALSE),
                                   '|',
                                   operandType (left)), 0, 1);
    case ',':
      return geniCodeRValue (right, FALSE);

    case CALL:
      return geniCodeCall (ast2iCode (tree->left, lvl+1),
                           tree->right, lvl);
    case LABEL:
      geniCodeLabel (ast2iCode (tree->left, lvl+1)->operand.symOperand);
      return ast2iCode (tree->right, lvl+1);

    case GOTO:
      geniCodeGoto (ast2iCode (tree->left, lvl+1)->operand.symOperand);
      return ast2iCode (tree->right, lvl+1);

    case FUNCTION:
      geniCodeFunctionBody (tree, lvl);
      return NULL;

    case RETURN:
      geniCodeReturn (right);
      return NULL;

    case IFX:
      geniCodeIfx (tree, lvl);
      return NULL;

    case SWITCH:
      geniCodeSwitch (tree, lvl);
      return NULL;

    case INLINEASM:
      geniCodeInline (tree);
      return NULL;

    case ARRAYINIT:
        geniCodeArrayInit(tree, ast2iCode (tree->left, lvl+1));
        return NULL;

    case CRITICAL:
        geniCodeCritical (tree, lvl);
    }

  return NULL;
}

/*-----------------------------------------------------------------*/
/* reverseICChain - gets from the list and creates a linkedlist    */
/*-----------------------------------------------------------------*/
iCode *
reverseiCChain ()
{
  iCode *loop = NULL;
  iCode *prev = NULL;

  while ((loop = getSet (&iCodeChain)))
    {
      loop->next = prev;
      if (prev)
        prev->prev = loop;
      prev = loop;
    }

  return prev;
}


/*-----------------------------------------------------------------*/
/* iCodeFromAst - given an ast will convert it to iCode            */
/*-----------------------------------------------------------------*/
iCode *
iCodeFromAst (ast * tree)
{
  returnLabel = newiTempLabel ("_return");
  entryLabel = newiTempLabel ("_entry");
  ast2iCode (tree,0);
  return reverseiCChain ();
}

static const char *opTypeToStr(OPTYPE op)
{
    switch(op)
    {
      case SYMBOL: return "symbol";
      case VALUE: return "value";
      case TYPE: return "type";
    }
    return "undefined type";
}


operand *validateOpType(operand         *op,
                        const char      *macro,
                        const char      *args,
                        OPTYPE          type,
                        const char      *file,
                        unsigned        line)
{
    if (op && op->type == type)
    {
        return op;
    }
    fprintf(stderr,
            "Internal error: validateOpType failed in %s(%s) @ %s:%u:"
            " expected %s, got %s\n",
            macro, args, file, line,
            opTypeToStr(type), op ? opTypeToStr(op->type) : "null op");
    exit(EXIT_FAILURE);
    return op; // never reached, makes compiler happy.
}
