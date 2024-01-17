
export async function getResString(com: any, r: Resource): Promise<string> {
  return await getResStringByContext(getContext(com), r)
}
export async function getResStringByContext(context: Context, r: Resource): Promise<string> {
  return await context.resourceManager.getStringValue(r)
}