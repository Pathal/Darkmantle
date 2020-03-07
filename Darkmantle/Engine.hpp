//
// Engine.hpp
// Pathal
//

#pragma once

#include <thread>             // std::thread
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable

#include "Resources.hpp"
#include "Actions.hpp"
#include "Actor.hpp"
#include "Enemy.hpp"
#include "Scene.hpp"

class Engine {
private:
    Engine( );
    static Engine* instance;
	
	// engine resources
    EventState* recycled_eventstates;
	int eventstate_pool_size = 0;
    std::vector<EventState *> eventstate_batches;
	std::mutex mutex_eventstate;
    
    TimelineEvent* recycled_timelineevents;
	int timelineevent_pool_size = 0;
    std::vector<TimelineEvent *> timelineevent_batches;
	std::mutex mutex_timeline;
	
	ActorState* recycled_actorstates;
	int actorstate_pool_size = 0;
    std::vector<ActorState *> actorstate_batches;
	std::mutex mutex_actorstate;

	
	// actor resources
	// TODO: Move these into an actor class
    std::vector<Attack *> actions;
	DamageBreakdown final_breakdown;
	
	// Tree constants, ie target enemy, etc
	Enemy * target;
	Scene * scene; // lists the actors and enemies
	
	// For threading the resource allocations
	int EVENTSTATE_RESERVE_MIN  = 50;
	int EVENTSTATE_RESERVE_SIZE = 100;
	
	// Verbose logging of the tree parsing
	bool verbose = true;
	

    
public:
    class Settings {
    public:
        enum END_TYPE {
            TIME,
            DAMAGE,
            END_TYPES
        } mode;
        float value;
        
        bool IsOver(EventState* event) {
            if(mode == END_TYPE::TIME) {
                if(event->timeline->time > value) return true;
                return false;
            } else if (mode == END_TYPE::DAMAGE) {
                if(event->breakdown.total_damage > value) return true;
                return false;
            }
        
            // If there's an error in the mode, we want to immediately end.
            return true;
        }
        
        Settings() {
            mode = END_TYPE::TIME;
            value = 10.0f;
        }
    } config;
    
    static Engine* getInstance();
    ~Engine();
    
    
    
    // -------------- Methods -------------- //
    
	// -- EventState -- //
	
    // Allocate memory for EventStates
    bool ReserveEvents(int num = 100);
    
    // Pops an EventState out of the recycling bin
    // allocates EventStates if all are exhausted
    EventState* PullEvent(EventState * e = nullptr);
    
    // Fill out the Actions vector
    void PopulateActions();
    
    // returns the next child, null if none exist
    EventState* GetNextChild(EventState * e = nullptr);
    
    // checks if there is another child to process
    bool HasNextChild(EventState * e = nullptr);
    
    // Sends the breakdown to the parent node
    void SendDataToParent(EventState * e = nullptr);
    
	// sends the given node to the recycling bin
	// ignores children, which should have already been recycled on their own
    void RecycleNode(EventState * e = nullptr);
	
	
	
	// -- TimelineEvent -- //
	
	// Allocate memory for EventStates
    bool ReserveTimelineEvents(int num = 100);
	
	// Pulls a TimelineEvent out of the recycling bin
	TimelineEvent* PullTimelineEvent(TimelineEvent * e = nullptr);
	
	// clones the timeline queue, and returns the head of the new queue
	TimelineEvent* CloneTimeline(TimelineEvent * e = nullptr);

	// Pops the first TimelineEvent out of the EventState's timeline
	TimelineEvent* GetNextTimelineEvent(EventState * e = nullptr);

	// Places the TLE into the recycling bin
	void RecycleTimelineEvent(TimelineEvent * e = nullptr);
	
	// adds to the state's timeline, in choronological order
	void InsertTimelineEventIntoState(TimelineEvent * e, EventState * state);

    
	
	// -- Core Engine Loop -- //
	
	// Takes a given action, and fills out the event state with the effects, and children for the node
	// this returns false on any problem
	bool ProcessNode(EventState * event = nullptr);
	
    // Process the APL and calculate the breakdown
    void Run(TimelineEvent * e = nullptr);
	
	void EventStateAllocator(Engine * e = nullptr);
	void TimelineEventAllocator(Engine * e = nullptr);
	void ActorStateAllocator(Engine * e = nullptr);
};
