#ifndef __PANELCOMMAND_H
#define __PANELCOMMAND_H

#include "mainui.h"

#include "testcase.h"

#include <vector>


class PanelCommand : public CommandPanelRes
{
private:
	typedef CommandPanelRes super;
	
public:
	PanelCommand(wxWindow* parent);
	
	void setTestCase(const TestCasePtr & testCase);
	
protected:
	virtual void onButtonPlayClicked( wxCommandEvent& event );
	virtual void onButtonPauseClicked( wxCommandEvent& event );
	virtual void onButtonResumeClicked( wxCommandEvent& event );
	virtual void onButtonResetClicked( wxCommandEvent& event );
	virtual void onCheckBoxUseFramesClicked( wxCommandEvent& event );
	virtual void onCheckBoxBackwardClicked( wxCommandEvent& event );
	virtual void onChoiceDurationSelected( wxCommandEvent& event );
	virtual void onChoiceDelaySelected( wxCommandEvent& event );
	virtual void onChoiceTimeScaleSelected( wxCommandEvent& event );
	virtual void onChoiceRepeatSelected( wxCommandEvent& event );
	virtual void onCheckBoxYoyoClicked( wxCommandEvent& event );
	virtual void onChoiceRepeatDelaySelected( wxCommandEvent& event );

private:
	void resetTimeChoicesUnit();
	void loadTweenParam();
	int getTimeChoiceSelection(wxChoice * choice);

private:
	TestCasePtr testCase;
	std::vector<wxChoice *> timeChoiceList;
	TweenParam tweenParam;
};


#endif