// See License.txt for copyright info

using UnrealBuildTool;
using System.Collections.Generic;

public class LD35Target : TargetRules
{
	public LD35Target(TargetInfo Target)
	{
		Type = TargetType.Game;
	}

	//
	// TargetRules interface.
	//

	public override void SetupBinaries(
		TargetInfo Target,
		ref List<UEBuildBinaryConfiguration> OutBuildBinaryConfigurations,
		ref List<string> OutExtraModuleNames
		)
	{
		OutExtraModuleNames.AddRange( new string[] { "LD35" } );
	}
}
