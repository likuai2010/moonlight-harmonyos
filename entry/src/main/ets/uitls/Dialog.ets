
import { Loading } from '../pages/compoments/Loading'


export function BuildDialog(builder: any, ): CustomDialogController{
  return new CustomDialogController({
    builder: Loading({
    }),
    autoCancel: false,
    alignment: DialogAlignment.Center,
    customStyle: true
  })
}