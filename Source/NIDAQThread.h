/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2019 Allen Institute for Brain Science and Open Ephys

    ------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


#ifndef __NIDAQTHREAD_H__
#define __NIDAQTHREAD_H__

#include <DataThreadHeaders.h>
#include <stdio.h>
#include <string.h>

#include "nidaq-api/NIDAQmx.h"
#include "NIDAQComponents.h"

class SourceNode;
class NIDAQThread;

class RecordingTimer : public Timer
{

public:

	RecordingTimer(NIDAQThread* t_);
	void timerCallback();

	NIDAQThread* thread;
};

/**

	Communicates with NI-DAQmx.

	@see DataThread, SourceNode

*/

class NIDAQThread : public DataThread, public Timer
{

public:

	NIDAQThread(SourceNode* sn);
	~NIDAQThread();

	bool updateBuffer();

	void updateChannels();

	/** Returns true if the data source is connected, false otherwise.*/
	bool foundInputSource();

	/** Returns version and serial number info for hardware and API as a string.*/
	String getInfoString();

	/** Returns version and serial number info for hardware and API as XML.*/
	XmlElement getInfoXml();

	void openConnection();

	/** Initializes data transfer.*/
	bool startAcquisition() override;

	/** Stops data transfer.*/
	bool stopAcquisition() override;

	// DataThread Methods

	int getNumAnalogInputs();
	int getNumDigitalInputs();

	/** Returns the number of virtual subprocessors this source can generate */
	unsigned int getNumSubProcessors() const override;

	/** Returns the number of continuous headstage channels the data source can provide.*/
	int getNumDataOutputs(DataChannel::DataChannelTypes type, int subProcessorIdx) const override;

	/** Returns the number of TTL channels that each subprocessor generates*/
	int getNumTTLOutputs(int subProcessorIdx) const override;

	/** Returns the sample rate of the data source.*/
	float getSampleRate(int subProcessorIdx) const override;

	/** Returns the volts per bit of the data source.*/
	float getBitVolts(const DataChannel* chan) const override;

	/** Used to set default channel names.*/
	void setDefaultChannelNames() override;

	/** Used to override default channel names.*/
	bool usesCustomNames() const override;

	/** Toggles between internal and external triggering. */
	void setTriggerMode(bool trigger);

	/** Toggles between saving to NPX file. */
	void setRecordMode(bool record);

	/** Toggles between auto-restart setting. */
	void setAutoRestart(bool restart);

	/** Starts data acquisition after a certain time.*/
	void timerCallback();

	/** Sets the currently selected input */
	void setSelectedInput();

	/** Starts recording.*/
	void startRecording();

	/** Stops recording.*/
	void stopRecording();

	/** Set directory for input */
	void setDirectoryForInput(int id, File directory);

	/** Get directory for input */
	File getDirectoryForInput(int id);

	float getFillPercentage(int id);

	CriticalSection* getMutex()
	{
		return &displayMutex;
	}

	static DataThread* createDataThread(SourceNode* sn);

	GenericEditor* createEditor(SourceNode* sn);

	ScopedPointer<ProgressBar> progressBar;
	double initializationProgress;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NIDAQThread);

private:

	NIDAQAPI api;

	bool inputAvailable;
	Array<AnalogIn> ai;
	Array<DigitalIn> di;

	bool isRecording;

	CriticalSection displayMutex;

	void closeConnection();

	Array<float> fillPercentage;

	RecordingTimer recordingTimer;

};

#endif  // __NIDAQTHREAD_H__