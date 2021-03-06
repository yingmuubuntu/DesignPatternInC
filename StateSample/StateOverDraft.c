
#include <stdio.h>
#include <apr_strings.h>
#include "StateOverDraft.h"
#include "Account.h"
#include "StateNormal.h"
#include "StateRestricted.h"

//属性、方法声明
static void StateCheck(IState *pState);

struct StateOverDraft_Fld
{
	apr_pool_t *m_pPool;

	IState m_state;

	Account *m_pAccount;
};

static void Free(IState **ppState)
{
	StateOverDraft_Free(&(StateOverDraft *)(*ppState)->pImplicit);
	*ppState = NULL;
}

static Account *GetAccount(IState *pState)
{
	StateOverDraft *pInst = (StateOverDraft *)pState->pImplicit;
	return pInst->pFld->m_pAccount;
}

static void SetAccount(IState *pState, Account *pAccount)
{
	StateOverDraft *pInst = (StateOverDraft *)pState->pImplicit;

	pInst->pFld->m_pAccount = pAccount;
}

static void Deposit(IState *pState, double dblAmount)
{
	StateOverDraft *pInst = (StateOverDraft *)pState->pImplicit;

	pInst->pFld->m_pAccount->SetBalance(pInst->pFld->m_pAccount, pInst->pFld->m_pAccount->GetBalance(pInst->pFld->m_pAccount) + dblAmount);
	StateCheck(pState);
}

static void Withdraw(IState *pState, double dblAmount)
{
	StateOverDraft *pInst = (StateOverDraft *)pState->pImplicit;

	pInst->pFld->m_pAccount->SetBalance(pInst->pFld->m_pAccount, pInst->pFld->m_pAccount->GetBalance(pInst->pFld->m_pAccount) - dblAmount);
	StateCheck(pState);
}

static void CalcInterest(IState *pState)
{
	//StateOverDraft *pInst = (StateOverDraft *)pState->pImplicit;
	printf("计算利息。\n");
}

static char *StateDesc(IState *pState, apr_pool_t *pPool)
{
	//StateOverDraft *pInst = (StateOverDraft *)pState->pImplicit;
	return apr_pstrdup(pPool, "OverDraft");
}

static void StateCheck(IState *pState)
{
	StateOverDraft *pInst = (StateOverDraft *)pState->pImplicit;

	double dblBalanceTmp = pInst->pFld->m_pAccount->GetBalance(pInst->pFld->m_pAccount);
	if (dblBalanceTmp > 0)
	{
        pInst->pFld->m_pAccount->SetState(pInst->pFld->m_pAccount, StateNormal2IState(StateNormal_New(apr_pool_parent_get(pInst->pFld->m_pPool))));
	}
	else if (dblBalanceTmp == -2000)
	{
        pInst->pFld->m_pAccount->SetState(pInst->pFld->m_pAccount, StateRestricted2IState(StateRestricted_New(apr_pool_parent_get(pInst->pFld->m_pPool))));
	}
	else if (dblBalanceTmp < -2000)
	{
		printf("操作受限！\n");
	}
}

StateOverDraft * StateOverDraft_New(apr_pool_t * pSupPool)
{
	apr_pool_t *pPool;
	apr_pool_create(&pPool, pSupPool);

	StateOverDraft *pInst = apr_palloc(pPool, sizeof(StateOverDraft));
	
	pInst->pFld = apr_palloc(pPool, sizeof(StateOverDraft_Fld));
	pInst->pFld->m_pPool = pPool;
	pInst->pFld->m_state.pImplicit = pInst;
	pInst->pFld->m_state.Free = Free;

	pInst->pFld->m_state.GetAccount = GetAccount;
	pInst->pFld->m_state.SetAccount = SetAccount;
	pInst->pFld->m_state.Deposit = Deposit;
	pInst->pFld->m_state.Withdraw = Withdraw;
	pInst->pFld->m_state.CalcInterest = CalcInterest;
	pInst->pFld->m_state.StateCheck = StateCheck;
	pInst->pFld->m_state.StateDesc = StateDesc;

    pInst->pFld->m_pAccount = NULL;

	return pInst;
}

IState * StateOverDraft2IState(StateOverDraft * pInst)
{
	return &(pInst->pFld->m_state);
}

void StateOverDraft_Free(StateOverDraft ** ppInst)
{
	apr_pool_destroy((*ppInst)->pFld->m_pPool);
	*ppInst = NULL;
}
