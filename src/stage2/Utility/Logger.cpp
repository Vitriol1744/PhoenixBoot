#include "Logger.hpp"

#include "Drivers/Terminal.hpp"

OutputStream Logger::outputStreams = OutputStream::eE9;

void Logger::Log(LogLevel logLevel, const char* message, ...)
{
    va_list args;
    va_start(args, message);
    Log(logLevel, message, args);
    va_end(args);
}
void Logger::Log(LogLevel logLevel, const char* message, va_list args)
{
    TerminalColor savedForegroundColor = Terminal::Get()->GetForegroundColor();
    TerminalColor savedBackgroundColor = Terminal::Get()->GetBackgroundColor();
    switch (logLevel)
    {
        case LogLevel::eTrace:  Terminal::Get()->SetColor(TerminalColor::eGreen, TerminalColor::eBlack);   break;
        case LogLevel::eInfo:   Terminal::Get()->SetColor(TerminalColor::eCyan, TerminalColor::eBlack);    break;
        case LogLevel::eWarn:   Terminal::Get()->SetColor(TerminalColor::eYellow, TerminalColor::eBlack);  break;
        case LogLevel::eError:  Terminal::Get()->SetColor(TerminalColor::eRed, TerminalColor::eBlack);     break;
        case LogLevel::eFatal:  Terminal::Get()->SetColor(TerminalColor::eWhite, TerminalColor::eRed);     break;
    }

    if (outputStreams & OutputStream::eTerminal)
        vprintf(OutputStream::eTerminal, message, args);
    if (outputStreams & OutputStream::eSerial)
        vprintf(OutputStream::eSerial, message, args);
    if (outputStreams & OutputStream::eE9)
        vprintf(OutputStream::eE9, message, args);
    
    Terminal::Get()->SetColor(savedForegroundColor, savedBackgroundColor);
}