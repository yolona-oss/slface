#include <sll/input.h>
#include <sll/base.h>

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

namespace SL {

	int
	read_accounts_data_from_file(const std::string path,
								 Stor& to,
								 const std::string fmt,
								 const ReadAccsFlag rafFlags)
	{
		int ret(0);
		Item *copyItem = new Item;
		std::fstream file;
		std::string line;

		//check format
		if (fmt.length() < 2 && fmt.length() > 4) {
			std::cerr << "Cant read accs from file: incorrect format!" << std::endl;
			return -1;
		}

		int funame = 0;
		int fkey   = 0;
		int femail = 0;
		int fpass  = 0;
		authentication::Type auth_type = authentication::UNDEFINED;
		for (int i = 0; i < fmt.length(); i++) {
			switch (fmt[i]) {
				case 'u':
				case 'U':
					funame++;
					break;
				case 'k':
				case 'K':
					fkey++;
					break;
				case 'e':
				case 'E':
					femail++;
					break;
				case 'p':
				case 'P':
					fpass++;
					break;
			}
		}

		if (funame > 1 || fkey > 1
				|| femail > 1 || fpass >1) {
			std::cerr << "Cant read accs file: invalid format!" << std::endl;
			return -1;
		}

		if ((femail && fpass) && (funame && fkey)) {
			auth_type = Item::PREFERED_AUTH_TYPE;
		} else if (funame && fkey) {
			auth_type = authentication::USERNAME;
		} else if (femail && fpass) {
			auth_type = authentication::EMAIL;
		} else {
			std::cerr << "Cant read accs file: invalid format!" << std::endl;
			return -1;
		}

		//check format
		/* for (int i = 0; i < fmt.length(); i++) { */
		/* } */

		file.open(path);
		if (!file.is_open()) {
			std::cerr << "Cant open file: " << path << std::endl;
			return -1;
		}

		int fmtI = 0;
		if (rafFlags & RAF_COLUMN)
		{
			while (std::getline(file, line))
			{
				if (rafFlags & RAF_SKIPNL) {
					if (line.length() == 0) {
						continue;
					}
				}

				switch (fmt[fmtI]) {
					case 'U': case 'u': copyItem->setUsername(line);
						break;

					case 'K': case 'k': copyItem->setKey(line);
						break;

					case 'E': case 'e': copyItem->setEMail(line);
						break;

					case 'P': case 'p': copyItem->setPassword(line);
						break;
				}

				if (fmtI < fmt.length()-1) {
					fmtI++;
				} else {
					fmtI = 0;
					copyItem->setAuthType(auth_type);
					to.addItem(copyItem);
					copyItem = new Item;
				}
			}
		}
		else if (rafFlags & RAF_ROW)
		{
			std::cerr << "Row read future nor realized yet!";
			ret = -1;
		}
		else {
			ret = -1;
			std::cerr << "Undefined read type" << std::endl;
		}

		file.close();

		return ret;
	}
};
