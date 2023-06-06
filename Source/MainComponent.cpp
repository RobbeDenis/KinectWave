#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
    : m_LeftWrist{ 0, 0, 0, 0 }
    , m_RightWrist{ 0, 0, 0, 0 }
    , m_UpdateIntervalMs{ 20 }
    , m_EnableUpdate{ false }
    , m_InfoPos{ 0, 0 }
    , m_SquareWidth{ 270 }
    , m_SquareSpacing{ 20 }
    , m_SquarePos{ 180, 20, 0, 0 }
{
    setSize (800, 600);

    initKinectInfoUI();

    initInputChannels();
    initKinect();

    enableUpdate(true);
}

void MainComponent::initKinectInfoUI()
{
    addAndMakeVisible(m_ConnectedLabel);
    m_ConnectedLabel.setFont(juce::Font(24.f, juce::Font::bold));
    m_ConnectedLabel.setText("not connected", juce::dontSendNotification);
    m_ConnectedLabel.setColour(juce::Label::textColourId, juce::Colours::red);

    addAndMakeVisible(m_InfoLX);
    m_InfoLX.setFont(juce::Font(20.f, juce::Font::bold));
    m_InfoLX.setText("LX", juce::dontSendNotification);
    m_InfoLX.setColour(juce::Label::textColourId, juce::Colours::lightgreen);

    addAndMakeVisible(m_InfoLY);
    m_InfoLY.setFont(juce::Font(20.f, juce::Font::bold));
    m_InfoLY.setText("LY", juce::dontSendNotification);
    m_InfoLY.setColour(juce::Label::textColourId, juce::Colours::lightgreen);

    addAndMakeVisible(m_InfoRX);
    m_InfoRX.setFont(juce::Font(20.f, juce::Font::bold));
    m_InfoRX.setText("RX", juce::dontSendNotification);
    m_InfoRX.setColour(juce::Label::textColourId, juce::Colours::lightgreen);

    addAndMakeVisible(m_InfoRY);
    m_InfoRY.setFont(juce::Font(20.f, juce::Font::bold));
    m_InfoRY.setText("RY", juce::dontSendNotification);
    m_InfoRY.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
}

void MainComponent::initKinect()
{
    m_pTracker = std::make_unique<KSkeletonTracker>();

    if (SUCCEEDED(m_pTracker->Init()))
    {
        m_ConnectedLabel.setText("connected", juce::dontSendNotification);
        m_ConnectedLabel.setColour(juce::Label::textColourId, juce::Colours::forestgreen);

        m_pTracker->SetAngle(5);
        m_pTracker->AddJointForTracking(NUI_SKELETON_POSITION_WRIST_LEFT);
        m_pTracker->AddJointForTracking(NUI_SKELETON_POSITION_WRIST_RIGHT);
    }
}

void MainComponent::initInputChannels()
{
    if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio)
        && !juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio,
            [&](bool granted) { setAudioChannels(granted ? 2 : 0, 2); });
    }
    else
    {
        setAudioChannels(2, 2);
    }
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    bufferToFill.clearActiveBufferRegion();
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    juce::Colour color{ 29, 37, 41};
    g.setColour(color);
    g.fillRect(static_cast<int>(m_SquarePos.x), static_cast<int>(m_SquarePos.y), m_SquareWidth, m_SquareWidth);
    g.fillRect(static_cast<int>(m_SquarePos.x) + m_SquareWidth + m_SquareSpacing, static_cast<int>(m_SquarePos.y), m_SquareWidth, m_SquareWidth);

}

void MainComponent::resized()
{
    // Kinect info
    const int offset{ 10 };
    m_ConnectedLabel.setBounds(m_InfoPos.x, m_InfoPos.y, 200, 50);
    m_InfoLX.setBounds(m_InfoPos.x, m_InfoPos.y + 22 + offset, 200, 50);
    m_InfoLY.setBounds(m_InfoPos.x, m_InfoPos.y + 44 + offset, 200, 50);
    m_InfoRX.setBounds(m_InfoPos.x, m_InfoPos.y + 66 + offset, 200, 50);
    m_InfoRY.setBounds(m_InfoPos.x, m_InfoPos.y + 88 + offset, 200, 50);
}

void MainComponent::timerCallback()
{
    m_pTracker->Update();
    m_LeftWrist = m_pTracker->GetTrackingJointPosition(NUI_SKELETON_POSITION_WRIST_LEFT);
    m_RightWrist = m_pTracker->GetTrackingJointPosition(NUI_SKELETON_POSITION_WRIST_RIGHT);

    m_LeftWrist.x = map(m_LeftWrist.x, -1, 0, 0, 1);
    m_LeftWrist.y = map(m_LeftWrist.y, -0.5, 0.5, 0, 1);
    m_RightWrist.x = map(m_RightWrist.x, -0.5, 0.5, 0, 1);
    m_RightWrist.y = map(m_RightWrist.y, -0.5, 0.5, 0, 1);

    juce::MessageManagerLock lock;
    m_InfoLX.setText("LX: " + juce::String::formatted("%f", m_LeftWrist.x), juce::dontSendNotification);
    m_InfoLY.setText("LY: " + juce::String::formatted("%f", m_LeftWrist.y), juce::dontSendNotification);
    m_InfoRX.setText("RX: " + juce::String::formatted("%f", m_RightWrist.x), juce::dontSendNotification);
    m_InfoRY.setText("RY: " + juce::String::formatted("%f", m_RightWrist.y), juce::dontSendNotification);
}

void MainComponent::enableUpdate(bool enable)
{
    m_EnableUpdate = enable;

    if (m_EnableUpdate)
        startTimer(m_UpdateIntervalMs);
    else
        stopTimer();
}

void MainComponent::setUpdateInterval(int intervalMs)
{
    m_UpdateIntervalMs = intervalMs;

    if (m_EnableUpdate)
    {
        stopTimer();
        startTimer(m_UpdateIntervalMs);
    }
}

float MainComponent::map(float value, float min, float max, float toMin, float toMax)
{
    value = std::clamp(value, min, max);
    return toMin + (value - min) * (toMax - toMin) / (max - min);
}