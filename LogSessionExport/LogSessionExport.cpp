/*
 * Copyright (c) 2015 PolySync
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <iostream>

#include <PolySyncApplication.hpp>
#include <PolySyncCore.hpp>
#include <PolySyncDataModel.hpp>

#include "LogSessionExport.hpp"


SessionExportExample::SessionExportExample(
        int sessionId,
        const std::string & sessionPath )
    :
    _sessionId( sessionId ),
    _sessionPath( sessionPath ),
    _exporter()
{
    // Subscribe to ApplicationEventMessage to determine when
    // the application connects to the PolySync bus.
    connectSubscriberMethod< SessionExportExample >(
            polysync::ApplicationEventMessage::getName(),
            &SessionExportExample::handleEvent,
            this );

    polysync::Application::getInstance()->attachSubscriber( this );
}


void SessionExportExample::handleEvent(
        std::shared_ptr< polysync::Message > message )
{
    if( auto event = polysync::datamodel::getSubclass<
                polysync::ApplicationEventMessage >( message ) )
    {
        if( event->getEventKind() == polysync::EventKind::Init )
        {
            // This is the actual usage of the export API.
            _exporter = std::unique_ptr< polysync::LogSessionExport >{
                    new polysync::LogSessionExport{
                        _sessionId, _sessionPath } };

            // Export can be started with or without a progress callback.
            // When complete, results will be located at
            // "PSYNC_HOME/rnr_logs/export/[sessionId]"
            _exporter->start(
                    this,
                    &SessionExportExample::handleTransferStatus );
        }
    }
}


void SessionExportExample::handleTransferStatus(
        const polysync::LogSessionTransferState state,
        const std::shared_ptr< polysync::datamodel::FileSyncStatusMessage > & )
{
    std::cout << "State: ";

    switch( state )
    {
        case polysync::LogSessionTransferState::Invalid:
            std::cout << "Invalid" << std::endl;
            break;
        case polysync::LogSessionTransferState::Error:
            std::cout << "Error" << std::endl;
            break;
        case polysync::LogSessionTransferState::Initial:
            std::cout << "Initial" << std::endl;
            break;
        case polysync::LogSessionTransferState::Enumeration:
            std::cout << "Enumeration" << std::endl;
            break;
        case polysync::LogSessionTransferState::TransferringSystemFiles:
            std::cout << "TransferringSystemFiles" << std::endl;
            break;
        case polysync::LogSessionTransferState::TransformingSystemFile:
            std::cout << "TransformingSystemFile" << std::endl;
            break;
        case polysync::LogSessionTransferState::TransferringLogfiles:
            std::cout << "TransferringLogfiles" << std::endl;
            break;
        case polysync::LogSessionTransferState::Complete:
            std::cout << "Complete" << std::endl;
            polysync::Application::getInstance()->disconnectPolySync();
            break;
        default:
            std::cout << "Unknown polysync::LogSessionTransferState"
                      << std::endl;
    }
}
