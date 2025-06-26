#pragma once
#include <memory>
#include <vector>

class Observer {
public:
	Observer() = default;
	virtual ~Observer() = default;
public:
	virtual void OnChanged() = 0;
};

class Observable {
public:
	Observable() = default;
	~Observable() = default;
public:
	void AddObserver( Observer* observer )
	{
		observers.push_back( observer );
	}
	void RemoveObserver( Observer* observer )
	{
		observers.erase( std::remove( observers.begin(), observers.end(), observer ), observers.end() );
	}
private:
	void NotifyObservers()
	{
		for( auto* observer : observers ) {
			if( observer )
				observer->OnChanged();
		}
	}
private:
	std::vector<Observer*> observers;
};
