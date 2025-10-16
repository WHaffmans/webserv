#pragma once

class HttpRequest;
class HttpResponse;

class AHandler
{
  public:
	AHandler(const HttpRequest &request, HttpResponse &response);
	virtual ~AHandler() = default;

	AHandler(const AHandler &other) = delete;
	AHandler &operator=(const AHandler &other) = delete;
	AHandler(AHandler &&other) noexcept = delete;
	AHandler &operator=(AHandler &&other) noexcept = delete;

    virtual void handle() = 0;
   
    protected:
      const HttpRequest &request_;
      HttpResponse &response_;




};