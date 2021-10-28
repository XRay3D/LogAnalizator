#include "Resources.Designer.h"

namespace LogicAnalyzer
{
	namespace Properties
	{
std::shared_ptr<System::Resources::ResourceManager> Resources::resourceMan;
std::shared_ptr<System::Globalization::CultureInfo> Resources::resourceCulture;

		Resources::Resources()
		{
		}

		const std::shared_ptr<System::Resources::ResourceManager> &Resources::getResourceManager()
		{
			if (Object::ReferenceEquals(resourceMan, nullptr))
			{
				std::shared_ptr<System::Resources::ResourceManager> temp = std::make_shared<System::Resources::ResourceManager>("LogicAnalyzer.Properties.Resources", Resources::typeid->Assembly);
				resourceMan = temp;
			}
			return resourceMan;
		}

		const std::shared_ptr<System::Globalization::CultureInfo> &Resources::getCulture()
		{
			return resourceCulture;
		}

		void Resources::setCulture(const std::shared_ptr<System::Globalization::CultureInfo> &value)
		{
			resourceCulture = value;
		}
	}
}
