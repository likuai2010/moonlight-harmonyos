
import { Loading } from '../pages/compoments/Loading'

export function BuildDialog(title): CustomDialogController{
  return new CustomDialogController({
    builder: Loading({ }),
    autoCancel: false,
    alignment: DialogAlignment.Center,
    customStyle: true
  })
}