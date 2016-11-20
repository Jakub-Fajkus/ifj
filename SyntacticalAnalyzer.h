//
// Created by Jakub Fajkus on 12.11.16.
//

#ifndef IFJ_SYNTACTICANALYZER_H
#define IFJ_SYNTACTICANALYZER_H

#include "SymbolTable.h"

typedef struct FRAME {
}FRAME;

/**
 * Run the whole syntactical analysis
 *
 * @param fileName
 */
void runSyntacticalAnalysis(char *fileName);
TOKEN *getCachedToken(tDLList *tokens);
void returnCachedTokens(tDLList *tokens, unsigned int count);
/**
 * Get all tokend from the lexical analyser
 *
 * @param fileName
 * @return
 */
tDLList* getAllTokens(char *fileName);

#endif //IFJ_SYNTACTICANALYZER_H
