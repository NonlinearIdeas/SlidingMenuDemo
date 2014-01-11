/********************************************************************
 * File   : MainScene.cpp
 * Project: ToolsDemo
 *
 ********************************************************************
 * Created on 9/21/13 By Nonlinear Ideas Inc.
 * Copyright (c) 2013 Nonlinear Ideas Inc. All rights reserved.
 ********************************************************************
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any
 * damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any
 * purpose, including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must
 *    not claim that you wrote the original software. If you use this
 *    software in a product, an acknowledgment in the product
 *    documentation would be appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and
 *    must not be misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source
 *    distribution.
 */

#include "MainScene.h"

#define ARROW_LEFT (-1)
#define ARROW_RIGHT (-2)

#define MENU_ITEMS_ACROSS 4
#define MENU_ITEMS_DOWN 5
#define MENU_ITEMS_PAGE (MENU_ITEMS_ACROSS*MENU_ITEMS_DOWN)
#define MENU_ITEMS_TOTAL 50

#define MENU_PAGES ((MENU_ITEMS_TOTAL/MENU_ITEMS_PAGE)+1)
#define MENU_FRACTION (ccp(0.8,0.8))
#define MENU_ANCHOR (ccp(0.5,0.5))

#define SLIDE_DURATION 1.0

MainScene::MainScene() :
   _menu(NULL)
{
}

MainScene::~MainScene()
{
}

static CCPoint CalculatePosition(int itemNum)
{
   CCSize scrSize = CCDirector::sharedDirector()->getWinSize();
   float Xs = scrSize.width;
   float Ys = scrSize.height;
   int gRows = MENU_ITEMS_DOWN;
   int gCols = MENU_ITEMS_ACROSS;
   int gBins = gRows*gCols;
   float Xb = MENU_FRACTION.x*Xs/gCols;
   float Yb = MENU_FRACTION.y*Ys/gRows;
   float Xa = MENU_ANCHOR.x * Xs;
   float Ya = MENU_ANCHOR.y * Ys;
   int page = itemNum / gBins;
   
   int binCol = itemNum % gCols;
   int binRow = (itemNum-page*gBins) / gCols;
   
   float xPos = binCol * Xb + Xb/2 + Xa - MENU_FRACTION.x*Xs/2 + page * Xs;
   float yPos = Ya - binRow*Yb - Yb/2 + MENU_FRACTION.y * Ys/2;
   
   CCPoint pos = ccp(xPos,yPos);
   
   return pos;
}

void MainScene::CreateMenu()
{
   if(_menu == NULL)
   {
      CCSize scrSize = CCDirector::sharedDirector()->getWinSize();
      
      _menu = CCMenu::create();
      _menu->setPosition(ccp(0,0));
      addChild(_menu);
      
      
      CCMenuItemFont* pItem;
      CCPoint position;
      
      // Create the next/back menu items.
      for(int page = 0; page < MENU_PAGES; page++)
      {
         // Create the Back/Forward buttons for the page.
         // Back arrow if there is a previous page.
         if(page > 0)
         {
            pItem = CCMenuItemFont::create("Back", this, menu_selector(MainScene::MenuCallback));
            pItem->setTag(ARROW_LEFT);
            position = ccp(page*scrSize.width + scrSize.width*0.1,scrSize.height*0.1);
            pItem->setPosition(position);
            pItem->setFontSize(35);
            pItem->setFontName("Arial");
            _menu->addChild(pItem);
         }
         if(page < (MENU_PAGES-1))
         {
            pItem = CCMenuItemFont::create("Next", this, menu_selector(MainScene::MenuCallback));
            pItem->setTag(ARROW_RIGHT);
            position = ccp(page*scrSize.width + scrSize.width*0.9,scrSize.height*0.1);
            pItem->setPosition(position);
            pItem->setFontSize(35);
            pItem->setFontName("Arial");
            _menu->addChild(pItem);
         }
      }
      // Create the actual items
      for(int idx = 0; idx < MENU_ITEMS_TOTAL; idx++)
      {
         char buffer[256];
         sprintf(buffer,"Item #%d",idx);
         pItem = CCMenuItemFont::create(buffer, this, menu_selector(MainScene::MenuCallback));
         pItem->setFontSize(35);
         pItem->setFontName("Arial");
         pItem->setTag(idx);
         position = CalculatePosition(idx);
         pItem->setPosition(position);
         _menu->addChild(pItem);
         
      }
   }
}


bool MainScene::init()
{

   return true;
}

MainScene* MainScene::create()
{
   MainScene *pRet = new MainScene();
   if (pRet && pRet->init())
   {
      pRet->autorelease();
      return pRet;
   }
   else
   {
      CC_SAFE_DELETE(pRet);
      return NULL;
   }
}

void MainScene::onEnter()
{
   CCScene::onEnter();
   CreateMenu();
}

void MainScene::onExit()
{
   CCScene::onExit();
}

void MainScene::onEnterTransitionDidFinish()
{
   CCScene::onEnterTransitionDidFinish();
}

void MainScene::onExitTransitionDidStart()
{
   CCScene::onExitTransitionDidStart();
   
}

void MainScene::SlidingDone()
{
   _sliding = false;
}

void MainScene::PageLeft()
{
   if(_sliding)
      return;
   _sliding = true;
   CCSize scrSize = CCDirector::sharedDirector()->getWinSize();
   CCFiniteTimeAction* act1 = CCMoveBy::create(SLIDE_DURATION, ccp(scrSize.width,0));
   CCFiniteTimeAction* act2 = CCCallFunc::create(this, callfunc_selector(MainScene::SlidingDone));
   _menu->runAction(CCSequence::create(act1,act2,NULL));
}

void MainScene::PageRight()
{
   if(_sliding)
      return;
   _sliding = true;
   CCSize scrSize = CCDirector::sharedDirector()->getWinSize();
   CCFiniteTimeAction* act1 = CCMoveBy::create(SLIDE_DURATION, ccp(-scrSize.width,0));
   CCFiniteTimeAction* act2 = CCCallFunc::create(this, callfunc_selector(MainScene::SlidingDone));
   _menu->runAction(CCSequence::create(act1,act2,NULL));
}

void MainScene::MenuCallback(CCObject* sender)
{
   // This is a very contrived example
   // for handling the menu items.
   // -1 ==> Left Arrow
   // -2 ==> Right Arrow
   // Anything else is a selection
   CCMenuItem* pMenuItem = (CCMenuItem*)sender;
   switch(pMenuItem->getTag())
   {
      case ARROW_LEFT:
         PageLeft();
         break;
      case ARROW_RIGHT:
         PageRight();
         break;
      default:
         CCLOG("Got Item %d Pressed",pMenuItem->getTag());
         break;
   }
}





