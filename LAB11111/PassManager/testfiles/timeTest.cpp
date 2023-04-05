void timeTest()
{
	time_t timeOne = time(0);
	cout << "Секунды от 01.01.1970 = " << timeOne << endl;

	struct tm* timeTmStruct = nullptr;
	char dateStr[60] = { 0 };

	const time_t now = time(NULL);
	timeTmStruct = localtime(&now);

	char timeFormat[] = "%d.%m.%Y %H:%M:%S, %A, %W";
	int bytesWrite = strftime(dateStr, sizeof(dateStr), timeFormat, timeTmStruct);

	cout << "\nТекущая дата в формате " << timeFormat << '\n';
	cout << dateStr << "\nlen = " << bytesWrite << " " << strlen(dateStr);
	cout << "\n\n";

	cout << "Обратно в unix time: " << mktime(timeTmStruct) << '\n';
	cout << "Mc с момента запуска программы: " << clock() << '\n';


	switch (timeTmStruct->tm_wday)
	{
	case 0: strcpy(dateStr + bytesWrite, " воскресенье"); break;
	case 1: strcpy(dateStr + bytesWrite, " понедельник"); break;
	case 2: strcpy(dateStr + bytesWrite, " вторник");     break;
	case 3: strcpy(dateStr + bytesWrite, " среда");       break;
	case 4: strcpy(dateStr + bytesWrite, " четверг");     break;
	case 5: strcpy(dateStr + bytesWrite, " пятница");     break;
	case 6: strcpy(dateStr + bytesWrite, " суббота");     break;
	}

	cout << dateStr << "\nlen = " << bytesWrite << " " << strlen(dateStr);

	//Если мы меняем структуру tm, то нужно перевести время в секунды
	// и опять посчитать tm
}

void myTimer()
{
	std::time_t now   = std::time(nullptr);
	std::tm beginTime = *std::localtime(&now);

	std::cout << "Введите время таймера MM:SS ";
	std::cin >> std::get_time(&beginTime, "%M:%S");

	int diff;
	time_t previous = time(0);
	int timer = beginTime.tm_min*60 + beginTime.tm_sec;

	cout << " " << toMinAndSeconds(timer) << '\r';

	while (timer > 0)
	{
		now  = time(0);
		diff = now - previous;

		if (diff >= 1)
		{
			previous = now;
			timer -= diff;

			cout << " " << toMinAndSeconds(timer) << '\r';
		}
	}
	cout << "DING! DING! DING!" << endl;
}
