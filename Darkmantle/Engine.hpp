//
// Engine.hpp
// Pathal
//

#pragma once

#include "Resources.hpp"

class Engine {
private:
    Engine( );
    static Engine* instance;
	
	// engine resources
    EventState* recycled_eventstates;
    std::vector<EventState *> eventstate_batches;
    
    TimelineEvent* recycled_timelineevents;
    std::vector<TimelineEvent *> timelineevent_batches;
	
	
	// actor resources
	// TODO: Move these into an actor class
    std::vector<Attack *> actions;

	DamageBreakdown final_breakdown;
	int resource_lower[NUMBER_OF_RESOURCES];
	int resource_upper[NUMBER_OF_RESOURCES];

    
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
    
    
    
    // -- Methods -- //
    
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
	
	
	// Pulls a TimelineEvent out of the recycling bin
	TimelineEvent* PullTimelineEvent(TimelineEvent * e = nullptr);
	
	// clones the timeline queue, and returns the head of the new queue
	TimelineEvent* CloneTimeline(TimelineEvent * e = nullptr);

	// Pops the first TimelineEvent out of the EventState's timeline
	TimelineEvent* GetNextTimelineEvent(EventState * e = nullptr);

	// Places the TLE into the recycling bin
	void RecycleTimelineEvent(TimelineEvent * e = nullptr);
	
	// 
	void InsertTimelineEventIntoState(TimelineEvent * e, EventState * state);

    
	
	// Takes a given action, and fills out the event state with the effects, and children for the node
	// this returns false on any problem
	bool ProcessNode(EventState * event = nullptr);
	
    // Process the APL and calculate the breakdown
    void Run(TimelineEvent * e = nullptr);
};
