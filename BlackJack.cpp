#define HIT 0
#define STAND 1
#define DOUBLE 2
#define SPLIT 3

#include<iostream>
#include<vector>
#include<map>
using namespace std;



struct dealerHand{
	double reward;
	double reward11;
	double rewardBlackjack;
};
class State
{
    public:
    int stateId;
    double stateValue;
    int handValue;
    bool softHand, canSplit, blackjack, busted, state21;

    double actionValue[4];
    int bestAction;

    State(){}
    ~State(){}

    State(int stateId,int handValue,bool softHand, bool canSplit)
    {
        this->stateId=stateId;
        this->handValue=handValue;
        this->stateValue = 0;
        this->softHand=softHand;
        this->canSplit=canSplit;
        this->bestAction=bestAction;
        this->blackjack=false;
        this->busted=false;
        this->state21=false;
    }
    int set21()
    {
        this->state21=true;
        return 0;
    }
    int setBusted()
    {
        this->busted=true;
        return 0;
    }
    int setBlackJack()
    {
        this->blackjack=true;
        return 0;
    }

};


double fp=4.0/13,q=(1-fp)/9;

vector< vector<State> > stateSpace;
vector<State> state;
vector<struct dealerHand>dealer(22);
map<int,State *> hval2soft;
map<int,State *> hval2hard;
map<int,State *> hval2pair;

//vector<vector<vector<double> > > transition;
//vector<vector<vector<double> > > reward;
vector< vector <double> > transHit;
vector< vector <double> > rewardHit;

int getSoftState(int sum)
{
    return hval2soft[sum]->stateId;
}

int getHardState(int sum)
{
    return hval2hard[sum]->stateId;
}

void initialize()
{
    /// Initializing HardHand without Ace as a 11
    int sid=0;

    int d=2;

   // while(d<=11)
    {
        int hval=5;
        while(hval<=21)
        {
            State s(sid,hval,false,false);  /// Not soft-hand , not split
            state.push_back(s);
            if(hval==21)
            {
                s.set21();
            }
            hval2hard[hval]=&s;
            sid++;
            hval++;
        }

        /// Initializing SoftHand with Ace as 11
        hval=13;
        while(hval<=21)
        {
            State s(sid,hval,true,false);   /// soft-hand , not split
            state.push_back(s);
            if(hval==21)
            {
                s.setBlackJack();
            }
            hval2soft[hval]=&s;
            sid++;
            hval++;
        }

        /// Initializing Pairs
        hval=2;
        while(hval<=10)
        {
            State s(sid,2*hval,false,true); /// Not soft-hand , split
            state.push_back(s);
            hval2pair[2*hval]=&s;
            sid++;
            hval++;
        }
        state[25].blackjack= true;
        State s(sid,12,true,true);  /// soft-hand , split
        state.push_back(s);
        hval2soft[12]=&s;
        hval2pair[12]=&s;
		sid++;
        State s1(sid,22,false,false);
        s1.setBusted();
        state.push_back(s1);
        hval2hard[22]=&s1;
        hval2soft[22]=&s1;
        hval2pair[22]=&s1;
		sid++;
    //    stateSpace.push_back(state);
        d++;
    }




}
void printStates()
{
	cout << "state id : state value : handValue : softHand : canSplit : blackJack : busted : state21 " << endl;
	for(int i = 0; i < state.size();i++)
	{
		cout << state[i].stateId << "  " << state[i].stateValue << " " << state[i].handValue << " " << state[i].softHand << " " << state[i].canSplit << " " << state[i].blackjack << " " << state[i].busted << " " << state[i].state21 << endl;
	}
}
void initializeDealerHandValue(vector<struct dealerHand> &dealer)
{
	for(int i = 0; i < dealer.size();i++)
	{
		dealer[i].reward = 0;
		dealer[i].reward11 = 0;
		dealer[i].rewardBlackjack = 0;
	}
}
void printDealeHand(vector<struct dealerHand> &dealer)
{
	
}
double calculateReward(int id,int dealerValue,double p)
{
	cout << "came here 1" << endl;
	if(id == 36) return -1;
	
	cout << "came here 2" << endl;
	initializeDealerHandValue(dealer);
	cout << "came here 3" << endl;
	double q = (1 - p)/9;
	double prob[] = {0,q,q,q,q,q,q,q,q,q,p,q};///prob. according to 0 1 2 3 4 5 6 7 8 9 10 11///
	//dealer[22] = 
	cout << "came here 4" << endl;
	dealer[22].reward = 1;
		
	if(id == 25)
	{
		dealer[21].rewardBlackjack = 0;
		dealer[21].reward = 1.5; 
	}
	else{
		dealer[21].rewardBlackjack = -1;
		dealer[21].reward = state[id].handValue < 21 ? -1 : 0;
	}
	cout << "came here 4" << endl;
	for(int value = 17; value <= 20 ;value++)
	{
		if(state[id].handValue < value){
			dealer[value].reward = -1;
			dealer[value].reward11 = -1;
		}
		else if(state[id].handValue == value){
			dealer[value].reward = 0;
			dealer[value].reward11 = 0; 
		}
		else{
			dealer[value].reward = 1;
			dealer[value].reward11 = 1;
		}
	}
	cout << "came here 5" << endl;
	/// Assuming each dealer hand has no ace as 11 and also no ace comes in dack/// 
	for(int value = 16 ; value>=2;value--)
	{
		/// Now find the expected return at this stage///
		for(int i = 2 ;i <= 10 ; i++)
		{
			/// dealer hand have no ace in it///
			if( value + i <= 21)
				dealer[value].reward += prob[i]*dealer[value+i].reward;
			else
			dealer[value].reward += prob[i]*dealer[22].reward;
			
		}
	}
	/// Now lets face the reality of Ace hitting///
	/// assuming these values have ace as 11//
	for(int value = 16; value >=2 ;value--)
	{
		/// No new ace comes///
		for(int i = 2; i <= 10;i++)
		{
			if(value + i <= 21)
			{
				dealer[value].reward11 += prob[i]*dealer[value+i].reward11;
			}
			else{
				if(value + i -11 + 1 <= 21)
				{
					dealer[value].reward11 += prob[i]*dealer[value+i - 11 + 1].reward;
				}
				else
					dealer[value].reward11 += prob[i]*dealer[22].reward;
			}
		}
		
		/// Ace comes///
		int i = 11;
		/// tackling both cases of value having ace as 11 and value having ace as not 11///
		if(value + i <= 21){
			dealer[value].reward += prob[i]*dealer[value+i].reward11;
			dealer[value].reward11 += prob[i]*dealer[22].reward;
		}
		else{
			if(value + i -11 +1 <= 21)
			{
				dealer[value].reward += prob[i]*dealer[value+i -11 +1].reward;
				dealer[value].reward11 += prob[i]*dealer[value+i -11 + 1].reward11;
			}
			else{
				dealer[value].reward += prob[i]*dealer[22].reward;
				if(value + i -11 + 1+ -11 + 1 <= 21)
					dealer[value].reward11 += prob[i]*dealer[value+i -20].reward;
				else
					dealer[value].reward11 += prob[i]*dealer[22].reward;
			}
		}
	}
	cout << "came here 6" << endl;
	if(dealerValue != 11){
		cout << "Came here if " << dealer[dealerValue].reward <<  endl;
		return dealer[dealerValue].reward;
	}
	else{
		cout << "came here else" << endl;
		return dealer[dealerValue].reward11;
	}
	//return 5.0;
}
int rewardFunctionHit(double p,vector<vector<double> > &rewardHit)
{
	double q = (1-p)/9;
	cout << "Size of state space "<< state.size() << endl;
	rewardHit.resize(state.size(),vector<double>(12)); //// Assuming the Hand values of ( NOT USEFUL 0 1) 2 3 4 5 6 7 8 9 10 A/////
	/// dealing with Hand Values 2 - 10 ///

	for(int dv = 2;dv <= 10;dv++)
	{
		for(int id = 0; id < 37;id++)
		{
			cout << "Hi " << endl;
			//double b = calculateReward(2,2,4.0/13.0);
			rewardHit[id][dv] = calculateReward(id,dv,p);
			cout <<"Reward for id and dv " << id << " " << dv  << " is : " << rewardHit[id][dv] << endl;
		}
	}
	cout << "came here finally " << endl;
	return 0;
}
int main()
{
	initialize();
	printStates();
	
	rewardFunctionHit(4/13,rewardHit);
    return 0;
}
