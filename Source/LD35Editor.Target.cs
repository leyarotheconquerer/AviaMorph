// See License.txt for copyright info

using UnrealBuildTool;
using System.Collections.Generic;

public class LD35EditorTarget : TargetRules
{
	public LD35EditorTarget(TargetInfo Target)
	{
		Type = TargetType.Editor;
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
