#include "Problem.h"

Problem::Problem( istream& ifs ) {

  // read in a problem instance from file with extension file format .tim

  ifs >> n_of_events;
  //cout << "n_of_events: " << n_of_events << endl;
  ifs >> n_of_rooms;
  //cout << "n_of_rooms: " << n_of_rooms << endl;
  ifs >> n_of_features;
  //cout << "n_of_features: " << n_of_features << endl;
  ifs >> n_of_students;
  //cout << "n_of_students: " << n_of_students << endl;

  // read room sizes
  int size;	
  for( int i = 0; i < n_of_rooms; i++ ) { 
    ifs >> size;
    roomSize.push_back(size);
    //cout << "Room " << i << ", size: " << roomSize[i];
  }
  //cout << endl;

  // read student attendance and keep it in a matrix
  student_events = IntMatrixAlloc(n_of_students,n_of_events);
  for (int i = 0; i < n_of_students; i++) {
    for (int j = 0; j < n_of_events; j++) {
      ifs >> student_events[i][j];
    }
  }

  // calculate the number of students for each event and store it in the studentNumber vector
  for (int i = 0; i < n_of_events; i++) {
    int sum = 0;
    for (int j = 0; j < n_of_students; j++) {
      sum = sum + student_events[j][i];
    }
    studentNumber.push_back( sum);
  }

  /*for (int i = 0; i < n_of_events; i++) {
    cout << "Event " << i << ", nr_of_stud: " << studentNumber[i] << endl;
    }*/

  // calculate event correlations in terms of students in common and store them in the eventCorrelations matrix
  eventCorrelations = IntMatrixAlloc(n_of_events,n_of_events);
  for (int i = 0; i < n_of_events; i++) {
    for (int j = 0; j < n_of_events; j++) {
      eventCorrelations[i][j] = 0;
    }
  }
  for (int i = 0; i < n_of_events; i++) {
    for (int j = 0; j < n_of_events; j++) {
      for (int k = 0; k < n_of_students; k++) {
	if ((student_events[k][i] == 1) && (student_events[k][j] == 1)) {
	   eventCorrelations[i][j] = 1;
	   break;
	}
      }    
    }
  }

  // read features satisfied by each room and store them in the room_features matrix
  room_features = IntMatrixAlloc(n_of_rooms,n_of_features);
  for (int i = 0; i < n_of_rooms; i++) {
    for (int j = 0; j < n_of_features; j++) {
      ifs >> room_features[i][j];
    }
  }

  // read features required by each event and store them in the event_features matrix
  event_features = IntMatrixAlloc(n_of_events,n_of_features);
  for (int i = 0; i < n_of_events; i++) {
    for (int j = 0; j < n_of_features; j++) {
      ifs >> event_features[i][j];
    }
  }

  /*for (int i = 0; i < n_of_rooms; i++) {
    cout << "Room " << i << " features " ;
    for (int j = 0; j < n_of_features; j++) {
      if( room_features[i][j] == 1)
	cout << j << " ";
    }
    cout << endl;
    }*/

  /*for (int i = 0; i < n_of_events; i++) {
    cout << "Event " << i << " features " ;
    for (int j = 0; j < n_of_features; j++) {
      if( event_features[i][j] == 1)
	cout << j << " ";
    }
    cout << endl;
    }*/

  // pre-process which rooms are suitable for each event 
 possibleRooms = IntMatrixAlloc(n_of_events,n_of_rooms);
 for (int i = 0; i < n_of_events; i++) {
   for (int j = 0; j < n_of_rooms; j++) {
     possibleRooms[i][j] = 0;
   }
 }
 int k = 0;
 for (int i = 0; i < n_of_events; i++) {
   for (int j = 0; j < n_of_rooms; j++){
     if((roomSize[j] >= studentNumber[i])){
       for(k = 0; k < n_of_features; k++){
	 if(event_features[i][k] == 1 && room_features[j][k] == 0)
	   break;
       }
       if(k == n_of_features)
	 possibleRooms[i][j] = 1;
     }
   }
 }
  
 /*for (int i = 0; i < n_of_events; i++) {
    cout << "Event " << i << " possible rooms " ;
    for (int j = 0; j < n_of_rooms; j++) {
      if( possibleRooms[i][j] == 1)
	cout << j << " ";
    }
    cout << endl;
    }*/
}

Problem::~Problem()
{
  free(eventCorrelations);
  free(room_features);
  free(event_features);
  free(possibleRooms);
}
